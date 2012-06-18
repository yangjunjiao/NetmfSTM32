/* Copyright (c) 2012 Oberon microsystems, Inc. (Switzerland)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

// Originally developed for the book
//   "Getting Started with the Internet of Things", by Cuno Pfister.
//   Copyright 2011 Cuno Pfister, Inc., 978-1-4493-9357-1.

// Version 1.0, for the .NET Micro Framework release 4.2.

// Server-specific parts of Gsiot.Server namespace.
// See the book "Getting Started with the Internet of Things", Appendix C.
//
// The following files provide helper namespace used by Gsiot.Server:
//     Gsiot.Contracts.cs
//     Gsiot.HttpRiders.cs
//     Gsiot.Yaler.cs

// This server implementation intentionally uses a single thread only, to
// make it simple, small, and with minimal memory overhead.
// If such a single-threaded server kept a connection to a client open,
// no requests from other clients could be handled. For this reason, the
// server explicitly closes the connection after handling a request, or
// after a timeout when no new data has arrived for a while.
// To explicitly close the connection after a request, the server sends
// the "Connection: close" header in its response.

using System;
using System.Collections;
using System.IO;
using System.Net.Sockets;       // only for SocketException
using System.Text;
using System.Threading;
using Gsiot.Contracts;
using Gsiot.HttpRiders;
using Gsiot.Listeners;
using Gsiot.Listeners.Tcp;
using Gsiot.Listeners.Yaler;
using Microsoft.SPOT;

namespace Gsiot.Server
{
    /// <summary>
    /// An instance of class RequestHandlerContext provides information
    /// about the received HTTP request to a request handler. The request
    /// handler uses it to set up the HTTP response to this request, and
    /// if necessary, to construct URIs to the same service.
    /// </summary>
    public class RequestHandlerContext
    {
        // To keep this class simple, the following holds:
        //   - It only supports the most important HTTP headers.
        //   - It only supports UTF8-based content types, with content
        //     represented as single strings.

        // server interface
        string serviceRoot;     // where ((serviceRoot != null) &&
        //   ("serviceRoot is an absolute URI")
        string relayDomain;     // null iff no relay is used

        /// <summary>
        /// Constructor of RequestHandlerContext.
        /// </summary>
        /// <param name="serviceRoot">The URI relative to which the
        /// request URIs are processed, e.g., http://192.168.5.100:8080.
        /// </param>
        /// <param name="relayDomain">Indicates whether a relay is used;
        /// otherwise, it is null.</param>
        public RequestHandlerContext(string serviceRoot,
            string relayDomain)
        {
            Contract.Requires(serviceRoot != null);
            Contract.Requires(serviceRoot.Substring(0, 7) == "http://");
            Contract.Requires(serviceRoot[serviceRoot.Length - 1] != '/');
            this.serviceRoot = serviceRoot;
            this.relayDomain = relayDomain;
        }

        /// <summary>
        /// Before a request handler is called, this property is set to
        /// true if (and only if) the received request contained a
        /// Connection: close header. If the request handler wants to
        /// indicate that it wants to close the connection, it can set
        /// the property to true, which will add the Connection: close
        /// header to its response.
        /// 
        /// Implementation note: This implementation always closes the
        /// connection after handling a request.
        /// </summary>
        public bool ConnectionClose { get; set; }

        // request interface

        string requestUri;

        /// <summary>
        /// This property tells you which kind of request has been
        /// received (an HTTP method such as GET or PUT). You only need
        /// to check this property if you want to support several HTTP
        /// methods in the same request handler, i.e., request patterns
        /// with a * wildcard at the beginning.
        /// </summary>
        public string RequestMethod { get; internal set; }

        /// <summary>
        /// This property contains the URI of the incoming request. You
        /// only need this property if you want to support several
        /// resources in the same request handler, i.e., request patterns
        /// with a * wildcard at the end.
        /// </summary>
        public string RequestUri
        {
            get { return requestUri; }

            internal set
            {
                Contract.Requires(value != null);
                Contract.Requires(value.Length > 0);
                Contract.Requires(value[0] == '/');
                if ((relayDomain != null) && (value == '/' + relayDomain))
                {   // After stripping away the relay prefix, this
                    // would be an illegal request URI (empty string).
                    value = value + '/';
                }
                requestUri = value;
            }
        }

        /// <summary>
        /// This property contains the content of the request’s
        /// Content-Length header if one was present; otherwise, it is
        /// null.
        /// </summary>
        public string RequestContentType { get; internal set; }

        /// <summary>
        /// This property contains the request message body converted into
        /// a string of text, with a UTF8 encoding. You only need this
        /// property for PUT and POST requests, since GET and DELETE have
        /// no message bodies.
        /// </summary>
        public string RequestContent { get; internal set; }

        internal bool RequestMatch(Element e)
        {
            Contract.Requires(e != null);
            Contract.Requires(e.Path != null);
            Contract.Requires(e.Path.Length > 0);
            Contract.Requires(e.Path[0] == '/');
            // Pattern = ( Method | '*') path [ '*' ]
            string uri = RequestUri;
            Contract.Requires(uri != null);
            int uriLength = uri.Length;
            Contract.Requires(uriLength >= 1);
            if (uri[0] != '/')      // some proxies return absolute URIs
            {
                return false;
            }
            string method = RequestMethod;
            Contract.Requires(method != null);
            int methodLength = method.Length;
            Contract.Requires(methodLength >= 3);
            if ((method != e.Method) && (e.Method != "*")) { return false; }

            var pos = 1;
            if (relayDomain != null)    // try to match relay domain
            {
                int relayPrefixLength = relayDomain.Length + 1;
                if (uriLength <= relayPrefixLength) { return false; }
                while (pos != relayPrefixLength)
                {
                    if (uri[pos] != relayDomain[pos - 1]) { return false; }
                    pos = pos + 1;
                }
                if (uri[pos] != '/') { return false; }
                pos = pos + 1;
            }
            // try to match request pattern
            int patternLength = e.Path.Length;
            if (uriLength < (pos - 1 + patternLength)) { return false; }
            var i = 1;
            while (i != patternLength)
            {
                if (uri[pos] != e.Path[i]) { return false; }
                pos = pos + 1;
                i = i + 1;
            }
            return ((pos == uriLength) || (e.Wildcard));
        }

        // server interface

        /// <summary>
        /// This method takes a path and constructs a relative URI out of
        /// it. If the request was relayed, this is taken into account.
        /// For example, BuildRequestUri("hello.html") may return
        /// /gsiot-FFMQ-TTD5/hello.html if the request pattern was
        /// "GET /hello*".
        /// You should use this method if your response contains relative
        /// hyperlinks to your server.
        /// 
        /// Preconditions
        ///     path != null
        ///     path.Length > 0
        ///     path[0] == '/'
        /// </summary>
        /// <param name="path">Relative path starting with a /.</param>
        /// <returns>The same string if no relay is used, or the
        /// same string prefixed with the relay domain otherwise.</returns>
        public string BuildRequestUri(string path)
        {
            Contract.Requires(path != null);
            Contract.Requires(path.Length > 0);
            Contract.Requires(path[0] == '/');
            return (relayDomain == null) ? path : "/" + relayDomain + path;
        }

        /// <summary>
        /// This method takes a path and constructs an absolute URI out of
        /// it. If the request was relayed, this is taken into account.
        /// For example, BuildAbsoluteRequestUri("hello.html") may return
        /// http://try.yaler.net/gsiot-FFMQ-TTD5/hello.html if the request
        /// pattern was "GET /hello*".
        /// You should use this method if your response contains absolute
        /// hyperlinks to your server.
        /// 
        /// Preconditions
        ///     path != null
        ///     path.Length > 0
        ///     path[0] == '/'
        /// </summary>
        /// <param name="path">Relative path starting with a /.</param>
        /// <returns>Absolute URI, containing relay domain if a relay
        /// is used.</returns>
        public string BuildAbsoluteRequestUri(string path)
        {
            Contract.Requires(path != null);
            Contract.Requires(path.Length > 0);
            Contract.Requires(path[0] == '/');
            return serviceRoot + BuildRequestUri(path);
        }

        // response interface

        int statusCode = 200;       // OK
        string responseContentType = "text/plain";
        int responseMaxAge = -1;    // no cache-control header

        /// <summary>
        /// This property can be set to indicate the status code of the
        /// response. The most important status codes for our purposes are:
        ///     200 (OK)
        ///     400 (Bad Request)
        ///     404 (Not Found)
        ///     405 (Method Not Allowed)
        /// </summary>
        public int ResponseStatusCode
        {
            get { return statusCode; }

            // -1 means "undefined value"
            set
            {
                Contract.Requires((value >= 100) || (value == -1));
                Contract.Requires(value < 600);
                statusCode = value;
            }
        }

        /// <summary>
        /// This property can be set to indicate the content type of the
        /// response. This so-called MIME type will become the value of
        /// the HTTP Content-Type header. The most important content
        /// types for our purposes are:
        /// • text/plain
        /// Used for a plain-text response such as a single numeric or
        /// text value.
        /// • text/csv
        /// Used to send a series of values.
        /// • text/html
        /// Used to send a response with formatted HTML.
        /// </summary>
        public string ResponseContentType
        {
            get { return responseContentType; }

            set
            {
                Contract.Requires(value != null);
                Contract.Requires(value.Length > 0);
                responseContentType = value;
            }
        }

        // TODO
        public int ResponseMaxAge
        {
            get { return responseMaxAge; }

            set
            {
                Contract.Requires(value >= -1);
                responseMaxAge = value;
            }
        }


        /// <summary>
        /// This property can be set with the content of the response
        /// message (message body). It will be encoded in UTF8.
        /// </summary>
        public string ResponseContent { get; set; }

        /// <summary>
        /// This method takes a string and sets up the response message
        /// body accordingly. Parameter textType indicates the content
        /// type, e.g., text/plain, text/html, etc. This method sets the
        /// response status code to 200 (OK).
        /// This method is provided for convenience so that status code,
        /// content, and content type need not be set separately.
        /// 
        /// Preconditions
        ///     content != null
        ///     textType != null
        ///     textType.Length > 0
        /// </summary>
        /// <param name="content">HTTP message body as a string, which
        /// will be encoded as UTF-8.</param>
        /// <param name="textType">A MIME type that consists of text.
        /// </param>
        public void SetResponse(string content, string textType)
        {
            Contract.Requires(content != null);
            Contract.Requires(textType != null);
            Contract.Requires(textType.Length > 0);
            ResponseStatusCode = 200;    // OK
            ResponseContentType = textType;
            ResponseContent = content;
        }
    }


    /// <summary>
    /// The delegate type RequestHandler determines the parameter
    /// (context) and result (void) that a method must have so that it
    /// can be added to a request routing collection.
    /// 
    /// Preconditions
    ///     context != null
    /// </summary>
    /// <param name="context">Context with its request and server state
    /// set up, but without response state yet.</param>
    public delegate void RequestHandler(RequestHandlerContext context);


    // RequestRouting

    class Element
    {
        internal Element next;
        internal string Method;
        internal string Path;
        internal bool Wildcard;
        internal RequestHandler Handler;

        internal Element(string method, string path, bool wildcard,
            RequestHandler handler)
        {
            Contract.Requires(method != null);
            Contract.Requires(method.Length >= 3);
            Contract.Requires(path != null);
            Contract.Requires(path.Length > 0);
            Contract.Requires(path[0] == '/');
            Contract.Requires(handler != null);
            Method = method;
            Path = path;
            Wildcard = wildcard;
            Handler = handler;
        }
    }

    /// <summary>
    /// An instance of class RequestRouting is automatically created as a
    /// property when a new HttpServer object is created. Because it
    /// implements the IEnumerable interface and provides an Add method,
    /// it supports C# collection initializers. This means that instead
    /// of explicitly calling the Add method with the parameters pattern
    /// and handler, an initializer with pattern and handler as elements
    /// can be used.
    /// </summary>
    public class RequestRouting : IEnumerable
    {
        Element first;

        class Enumerator : IEnumerator
        {
            Element first;
            Element current;

            internal Enumerator(Element first)
            {
                this.first = first;
            }

            object IEnumerator.Current
            {
                get
                {
                    return current;
                }
            }

            bool IEnumerator.MoveNext()
            {
                if (current == null)
                {
                    current = first;
                }
                else
                {
                    current = current.next;
                }
                return (current != null);
            }

            void IEnumerator.Reset()
            {
                current = null;
            }
        }

        public IEnumerator GetEnumerator()
        {
            return new Enumerator(first);
        }

        /// <summary>
        /// This method adds a new request routing element to the
        /// collection, consisting of a request pattern and a request
        /// handler.
        /// 
        /// Preconditions
        ///     pattern != null
        ///     pattern.Length >= 3
        ///     handler != null
        /// </summary>
        /// <param name="pattern">Request pattern (method, path).</param>
        /// <param name="handler">Request handler.</param>
        public void Add(string pattern, RequestHandler handler)
        {
            Contract.Requires(pattern != null);
            Contract.Requires(pattern.Length >= 3);
            Contract.Requires(handler != null);
            var e = Parse(pattern, handler);
            Contract.Requires(e != null);
            if (first == null)
            {
                first = e;
            }
            else
            {
                Element h = first;
                while (h.next != null) { h = h.next; }
                h.next = e;
            }
        }

        Element Parse(string p, RequestHandler handler)
        {
            string method;
            string path;
            bool wildcard;
            string[] s = p.Split(' ');
            if ((s.Length != 2) ||
                (s[0].Length <= 0) ||
                (s[1].Length <= 0))
            {
                return null;
            }
            method = s[0];
            int index = s[1].IndexOf('*');
            if (index == -1)
            {
                wildcard = false;
                path = s[1];
            }
            else if (index != s[1].Length - 1)
            {
                return null;
            }
            else
            {
                wildcard = true;
                path = s[1].Substring(0, index);
            }
            return new Element(method, path, wildcard, handler);
        }
    }


    // HTTP server

    /// <summary>
    /// An instance of class HttpServer represents a web service that
    /// handles HTTP requests at a particular port, or uses a relay server
    /// to make it accessible even without a public Internet address.
    /// </summary>
    public class HttpServer
    {
        /// <summary>
        ///  TODO
        /// </summary>
        public Listener Listener { get; set; }

        /// <summary>
        /// Optional property, which is set to 80 by default. If the
        /// server does not use a relay, this property indicates the
        /// port for which the server handles incoming HTTP requests.
        /// </summary>
        public int Port { get; set; }

        /// <summary>
        /// Optional property, which is set to try.yaler.net by default.
        /// If the server uses a relay, this property indicates the
        /// address of the relay.
        /// If the server does not use a relay (see RelayDomain below),
        /// this property is ignored.
        /// </summary>
        public string RelayHost { get; set; }

        /// <summary>
        /// Optional property, which determines whether a relay is used,
        /// and if one is used, what domain name is registered at the
        /// relay. By default, it is null.
        /// </summary>
        public string RelayDomain { get; set; }

        /// <summary>
        /// Mandatory property if a relay is used. The key is used for
        /// authenticating the device at the relay. The secret key is
        /// never sent over the network.
        /// If the server does not use a relay (see RelayDomain above),
        /// this property is ignored.
        /// </summary>
        public string RelaySecretKey { get; set; }

        RequestRouting requestRouting = new RequestRouting();

        /// <summary>
        /// Mandatory property. At least one request routing element
        /// should be added to this property to support at least one
        /// request URI.
        /// NEW
        /// When the server instance is created, an empty RequestRouting
        /// instance is created, so this property usually need not be
        /// set by the client.
        /// </summary>
        public RequestRouting RequestRouting
        {
            get { return requestRouting; }
            set { requestRouting = value; }
        }

        // diagnostic information
        public ServerDiagnostics Diagnostics { get; set; }

        DateTime startTime = DateTime.Now;

        public TimeSpan Uptime
        {
            get { return DateTime.Now - startTime; }
        }

        /// <summary>
        /// TODO
        /// </summary>
        public bool IsOpen { get; private set; }

        /// <summary>
        /// This method completes the initialization of the server. If a
        /// relay is used, it performs the first registration of the
        /// device at the relay. Before it is called, the server
        /// properties must have been set up. Normally, you don’t need to
        /// call this method, since it is called by Run if necessary.
        /// </summary>
        public void Open()
        {
            if (!IsOpen)
            {
                try
                {
                    if (Port == 0) { Port = 80; }
                    if (Listener == null)
                    {
                        if (RelayDomain == null)    // don't use a relay
                        {
                            Listener = new TcpListener
                            {
                                Port = Port,
                                Backlog = 6
                            };
                        }
                        else                        // use a relay
                        {
                            Contract.Requires(RelaySecretKey != null);
                            if ((RelayDomain == "gsioT-FFMQ-TTD5") ||
                                (RelayDomain == "<insert your relay domain here>"))
                            {
                                throw new ContractViolation
                                    ("Please use your own relay domain");
                                // See http://www.gsiot.info/yaler/ for more
                                // information on how to get your own relay domain
                                // and secret relay key.
                            }
                            if (RelayHost == null) { RelayHost = "try.yaler.net"; }
                            Listener = new YalerListener
                            {
                                Host = RelayHost,
                                Port = Port,
                                Domain = RelayDomain
                            };
                        }
                    }
                    Contract.Assert(Listener != null);
                    Listener.Open();
                    Debug.Print("Base Uri: " + Listener.ServiceRoot + "/");
                    IsOpen = true;
                }
                catch (ContractViolation e)
                {
                    Debug.Print("bug in Open:\r\n" + e);
                    throw;      // don't mask contract violations here
                }
                catch (Exception e)
                {
                    Debug.Print("exception in Open:\r\n" + e);
                    Close();
                }
            }
        }

        public void Close()
        {
            IsOpen = false;
            Contract.Assert(Listener != null);
            Listener.Close();
        }

        /// <summary>
        /// This method calls Open if it was not called already by the
        /// application, and then enters an endless loop where it
        /// repeatedly waits for incoming requests, accepts them, and
        /// performs the necessary processing for handling the request.
        /// </summary>
        public void Run()
        {
            if (!IsOpen) { Open(); }
            while (IsOpen)
            {
                Stream connection = null;
                // wait for next request
                while (connection == null)
                {
                    try
                    {
                        connection = Listener.Accept();
                        Contract.Assert(connection != null);
                    }
                    catch (IOException e)
                    {
                        Contract.Assert(connection == null);
                        // possibly device was disconnected
                        Debug.Print("IO error in Accept:\r\n" + e.Message);
                        if (Diagnostics != null)
                        {
                            Diagnostics.AcceptErrors = Diagnostics.AcceptErrors + 1;
                        }
                        Thread.Sleep(500);     // TODO does it make sense like this?
                    }
                    catch (SocketException e)
                    {
                        Contract.Assert(connection == null);
                        // possibly Internet was disconnected
                        Debug.Print("Socket error in Accept:\r\n" + e.Message);
                        if (Diagnostics != null)
                        {
                            Diagnostics.AcceptErrors = Diagnostics.AcceptErrors + 1;
                        }
                        Thread.Sleep(500);     // TODO does it make sense like this?
                    }
                    catch (ContractViolation e)
                    {
                        Contract.Assert(connection == null);
                        Debug.Print("bug in Accept:\r\n" + e);
                        if (Diagnostics != null)
                        {
                            Diagnostics.AcceptFailures = Diagnostics.AcceptFailures + 1;
                        }
                        Thread.Sleep(500);     // TODO does it make sense like this?
                    }
                    catch (Exception e)
                    {
                        Contract.Assert(connection == null);
                        Debug.Print("exception in Accept:\r\n" + e);
                        if (Diagnostics != null)
                        {
                            Diagnostics.AcceptFailures = Diagnostics.AcceptFailures + 1;
                        }
                        Thread.Sleep(500);     // TODO does it make sense like this?
                    }
                }
                // handle request
                var connectionClose = true;
                // ignore connectionClose, i.e., always
                // close connection after a request
                try
                {
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestsTotal = Diagnostics.RequestsTotal + 1;
                    }
                    ConsumeRequest(connection, Listener.ServiceRoot,
                        RelayDomain, RequestRouting,
                        ref connectionClose);
                }
                catch (IOException e)
                {
                    // possibly device was disconnected, or host has sent no data (read timeout)
                    Debug.Print("IO error in ConsumeRequest:\r\n" + e.Message);
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestHandlerErrors = Diagnostics.RequestHandlerErrors + 1;
                    }
                }
                catch (SocketException e)
                {
                    // possibly Internet was disconnected, or host has sent no data (read timeout)
                    Debug.Print("Socket error in ConsumeRequest:\r\n" + e.ErrorCode);
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestHandlerErrors = Diagnostics.RequestHandlerErrors + 1;
                    }
                }
                catch (ContractViolation e)
                {
                    Debug.Print("bug in ConsumeRequest:\r\n" + e);
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestHandlerFailures = Diagnostics.RequestHandlerFailures + 1;
                    }
                }
                //TODO remove comments
                //catch (Exception e)
                //{
                //    Debug.Print("exception in ConsumeRequest:\r\n" + e);
                //    if (Diagnostics != null)
                //    {
                //        Diagnostics.RequestHandlerFailures = Diagnostics.RequestHandlerFailures + 1;
                //    }
                //}
                connection.Close();
            }
        }


        /// <summary>
        /// Private method that handles an incoming request.
        /// It sets up a RequestHandlerContext instance with the data from
        /// the incoming HTTP request, finds a suitable request handler to
        /// produce the response, and then sends the response as an HTTP
        /// response back to the client.
        /// Preconditions
        ///     "connection is open"
        ///     serviceRoot != null
        ///     serviceRoot.Length > 8
        ///     "serviceRoot starts with 'http://' and ends with '/'"
        ///     requestRouting != null
        /// </summary>
        /// <param name="connection">Open TCP/IP connection</param>
        /// <param name="serviceRoot">The absolute URI that is a prefix of
        /// all request URIs that this web service supports. It must start
        /// with "http://" and must end with "/".</param>
        /// <param name="relayDomain">Host name or Internet address of the
        /// relay to be used, or null if no relay is used</param>
        /// <param name="requestRouting">Collection of
        ///   { request pattern, request handler}
        /// pairs</param>
        /// <param name="connectionClose">Return parameter that indicates
        /// that the connection should be closed after this call. This may
        /// be because the incoming request has a "Connection: close"
        /// header, because the request handler has set the
        /// ConnectionClose property, or because some error occurred.
        /// </param>
        internal static void ConsumeRequest(Stream connection,
            string serviceRoot, string relayDomain,
            RequestRouting requestRouting,
            ref bool connectionClose)
        {
            Contract.Requires(connection != null);
            Contract.Requires(serviceRoot != null);
            Contract.Requires(serviceRoot.Length > 8);
            Contract.Requires(serviceRoot.Substring(0, 7) == "http://");
            Contract.Requires(serviceRoot[serviceRoot.Length - 1] != '/');
            Contract.Requires(requestRouting != null);

            // initialization --------------------------------------------
            HttpReader reader = new HttpReader();
            HttpWriter writer = new HttpWriter();
            var context = new RequestHandlerContext(serviceRoot,
                relayDomain);
            // Both client and server may request closing the connection.
            // Initially, we assume that neither one wants to close the
            // connection.
            context.ConnectionClose = false;

            // receive request -------------------------------------------
            reader.Attach(connection);

            // request line
            string httpMethod;
            string requestUri;
            string httpVersion;

            reader.ReadStringToBlank(out httpMethod);
            reader.ReadStringToBlank(out requestUri);
            reader.ReadFieldValue(out httpVersion);
            if (reader.Status != HttpStatus.BeforeContent)  // error
            {
                reader.Detach();
                connectionClose = true;
                return;
            }

            context.RequestMethod = httpMethod;
            context.RequestUri = requestUri;
            // ignore version

            // headers
            string fieldName;
            string fieldValue;
            int requestContentLength = -1;
            reader.ReadFieldName(out fieldName);
            while (reader.Status == HttpStatus.BeforeContent)
            {
                reader.ReadFieldValue(out fieldValue);
                if (fieldValue != null)
                {
                    Contract.Assert(reader.Status ==
                                    HttpStatus.BeforeContent);
                    if (fieldName == "Connection")
                    {
                        context.ConnectionClose =
                            ((fieldValue == "close") ||
                             (fieldValue == "Close"));
                    }
                    else if (fieldName == "Content-Type")
                    {
                        context.RequestContentType = fieldValue;
                    }
                    else if (fieldName == "Content-Length")
                    {
                        if (Utilities.TryParseUInt32(fieldValue,
                            out requestContentLength))
                        {
                            // content length is now known
                        }
                        else
                        {
                            reader.Status = HttpStatus.SyntaxError;
                            reader.Detach();
                            connectionClose = true;
                            return;
                        }
                    }
                }
                else
                {
                    // it's ok to skip header whose value is too long
                }
                Contract.Assert(reader.Status == HttpStatus.BeforeContent);
                reader.ReadFieldName(out fieldName);
            }
            if (reader.Status != HttpStatus.InContent)
            {
                reader.Detach();
                connectionClose = true;
                return;
            }

            // content
            if (requestContentLength > 0)
            {
                // receive content
                var requestContent = new byte[requestContentLength];
                int toRead = requestContentLength;
                var read = 0;
                while ((toRead > 0) && (read >= 0))
                {
                    // already read: requestContentLength - toRead
                    read = reader.ReadContent(requestContent,
                        requestContentLength - toRead, toRead);
                    if (read < 0) { break; }    // timeout or shutdown
                    toRead = toRead - read;
                }
                try
                {
                    char[] chars = Encoding.UTF8.GetChars(requestContent);
                    context.RequestContent = new string(chars);
                }
                catch (Exception)
                {
                    context.RequestContentType = "text/plain";
                    context.RequestContent = "request content is not in UTF8 format";
                    Debug.Print(context.RequestContent);
                    // TODO signal wrong content format through HTTP status code 400 (Bad Request)?
                }
            }

            reader.Detach();
            if (reader.Status != HttpStatus.InContent)
            {
                connectionClose = true;
                return;
            }

            // delegate request processing to a request handler ----------
            var match = false;
            foreach (Element e in requestRouting)
            {
                if (context.RequestMatch(e))
                {
                    bool connClose = context.ConnectionClose;
                    context.ResponseStatusCode = -1;                                    // undefined
                    Contract.Requires(context.ResponseContentType != null);
                    Contract.Requires(context.ResponseContentType == "text/plain");     // default
                    try
                    {
                        e.Handler(context);
                    }
                    catch (Exception h)
                    {
                        Debug.Print("exception in request handler: " + h);    // TODO how to better handle handler exceptions?
                        throw;                                  // rethrow, to avoid masking of errors
                    }
                    Contract.Ensures(context.ResponseStatusCode >= 100);
                    Contract.Ensures(context.ResponseStatusCode < 600);
                    Contract.Ensures(context.ResponseContentType != null);
                    Contract.Ensures(context.ResponseContentType.Length > 0);
                    // make sure that handler has not reset connectionClose flag:
                    Contract.Ensures((!connClose) || (context.ConnectionClose));
                    match = true;
                    break;
                }
            }
            if (!match)
            {
                context.ResponseStatusCode = 404;    // Not Found
                context.ResponseContentType = "text/plain";
                context.ResponseContent = null;
            }

            uint availableMemory = Debug.GC(true);
            Debug.Print(context.RequestMethod + " " +
                        context.RequestUri + " -> " +
                        context.ResponseStatusCode + " [" + availableMemory + "]");

            // send response ---------------------------------------------
            writer.Attach(connection);

            // status line
            writer.WriteString("HTTP/1.1 ");
            writer.WriteString(context.ResponseStatusCode.ToString());
            writer.WriteLine(" ");  // omit optional reason phrase

            // headers
            if (connectionClose)   // TODO (context.ConnectionClose)
            {
                writer.WriteLine("Connection: close");
            }
            byte[] responseBuffer = null;
            var responseLength = 0;
            if (context.ResponseContent != null)
            {
                responseBuffer =
                    Encoding.UTF8.GetBytes(context.ResponseContent);
                responseLength = responseBuffer.Length;
                writer.WriteString("Content-Type: ");
                writer.WriteLine(context.ResponseContentType);
            }
            else
            {
                responseLength = 0;
            }
            writer.WriteString("Content-Length: ");
            writer.WriteLine(responseLength.ToString());
            if (context.ResponseMaxAge > 0)
            {
                writer.WriteLine("Cache-Control: max-age=" + context.ResponseMaxAge);
            }
            else if (context.ResponseMaxAge == 0)
            {
                writer.WriteLine("Cache-Control: no-cache");
            }

            // content
            writer.WriteBeginOfContent();
            if (context.ResponseContent != null)    // send content
            {
                writer.WriteContent(responseBuffer, 0, responseLength);
            }

            writer.Detach();
            connectionClose = context.ConnectionClose;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void ProcessRequest(object state)
        {
            Contract.Requires(state != null);
            Contract.Requires(state is Stream);
            var connection = (Stream)state;

            var i = 0;
            while (connection != null)
            {
                Debug.Print("process request " + i + " on this connection");
                i = i + 1;

                // handle request
                try
                {
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestsTotal = Diagnostics.RequestsTotal + 1;
                    }
                    var connectionClose = false;
                    // ignore connectionClose, i.e., always
                    // close connection after a request
                    ConsumeRequest(connection, Listener.ServiceRoot,
                        RelayDomain, RequestRouting,
                        ref connectionClose);
                    if (connectionClose)
                    {
                        connection.Close();
                        connection = null;
                    }
                }
                catch (IOException e)
                {
                    // possibly device was disconnected, or host has sent no data (read timeout)
                    Debug.Print("IO error in ConsumeRequest:\r\n" + e.Message);
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestHandlerErrors = Diagnostics.RequestHandlerErrors + 1;
                    }
                    connection.Close();
                    connection = null;
                }
                catch (SocketException e)
                {
                    // possibly Internet was disconnected, or host has sent no data (read timeout)
                    Debug.Print("Socket error in ConsumeRequest:\r\n" + e.ErrorCode);
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestHandlerErrors = Diagnostics.RequestHandlerErrors + 1;
                    }
                    connection.Close();
                    connection = null;
                }
                catch (ContractViolation e)
                {
                    Debug.Print("bug in ConsumeRequest:\r\n" + e);
                    if (Diagnostics != null)
                    {
                        Diagnostics.RequestHandlerFailures = Diagnostics.RequestHandlerFailures + 1;
                    }
                    connection.Close();
                    connection = null;
                }
                //TODO remove comments
                //catch (Exception e)
                //{
                //    Debug.Print("exception in ConsumeRequest:\r\n" + e);
                //    if (Diagnostics != null)
                //    {
                //        Diagnostics.RequestHandlerFailures = Diagnostics.RequestHandlerFailures + 1;
                //    }
                //}
            }
        }

        public void Start(int nofThreads)
        {
            Contract.Requires(nofThreads > 0);
            ThreadPool.Open(nofThreads);
            ThreadPool.Start();
            if (!IsOpen) { Open(); }
            while (IsOpen)
            {
                // wait for next request
                Stream connection = null;
                try
                {
                    Debug.Print("before accept");
                    connection = Listener.Accept();
                    Debug.Print("after accept");
                    Contract.Ensures(connection != null);
                }
                catch (IOException e)
                {
                    Contract.Assert(connection == null);
                    // possibly device was disconnected
                    Debug.Print("IO error in Accept:\r\n" + e.Message);
                    if (Diagnostics != null)
                    {
                        Diagnostics.AcceptErrors = Diagnostics.AcceptErrors + 1;
                    }
                    Thread.Sleep(500);     // TODO does it make sense like this?
                }
                catch (SocketException e)
                {
                    Contract.Assert(connection == null);
                    // possibly Internet was disconnected
                    Debug.Print("Socket error in Accept:\r\n" + e.Message);
                    if (Diagnostics != null)
                    {
                        Diagnostics.AcceptErrors = Diagnostics.AcceptErrors + 1;
                    }
                    Thread.Sleep(500);     // TODO does it make sense like this?
                }
                catch (ContractViolation e)
                {
                    Contract.Assert(connection == null);
                    Debug.Print("bug in Accept:\r\n" + e);
                    if (Diagnostics != null)
                    {
                        Diagnostics.AcceptFailures = Diagnostics.AcceptFailures + 1;
                    }
                    Thread.Sleep(500);     // TODO does it make sense like this?
                }
                catch (Exception e)
                {
                    Contract.Assert(connection == null);
                    Debug.Print("exception in Accept:\r\n" + e);
                    if (Diagnostics != null)
                    {
                        Diagnostics.AcceptFailures = Diagnostics.AcceptFailures + 1;
                    }
                    Thread.Sleep(500);     // TODO does it make sense like this?
                }
                if (connection != null)
                {
                    Debug.Print("connection was accepted");
                    ThreadPool.QueueUserWorkItem(ProcessRequest, connection);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    internal class Utilities
    {
        internal static bool TryParseUInt32(string s, out int result)
        {
            Contract.Requires(s != null);
            result = 0;
            if (s.Length > 0)
            {
                var r = 0;
                foreach (char c in s)
                {
                    if ((c < '0') || (c > '9')) { return false; }
                    var n = (int)(c - '0');
                    r = (r * 10) + n;
                }
                result = r;
                return true;
            }
            return false;
        }
    }
}
