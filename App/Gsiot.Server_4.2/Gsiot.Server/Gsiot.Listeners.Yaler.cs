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

// This is a variant of the asynchronous Yaler client library published at
//    http://hg.yaler.org/yalercontrib/src/a8d40553a4d9/CSharp/Library/

using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using Gsiot.Contracts;
using Microsoft.SPOT;
using Microsoft.SPOT.Net.NetworkInformation;

namespace Gsiot.Listeners.Yaler
{
    /// <summary>
    /// Object that emulates a listener socket (incoming) by opening an
    /// outgoing socket to a relay server. Once the initialization protocol
    /// has completed (upgrade from HTTP to PTTH protocol for reverse HTTP)
    /// then the socket is returned to the server application, which
    /// awaits incoming requests on this socket.
    /// </summary>
    public sealed class YalerListener : Listener
    {
        public bool IsOpen { get; private set; }
        public string Host { get; set; }
        public int Port { get; set; }
        public string Domain { get; set; }

        Socket listener;
        string serviceRoot;
        bool aborted;
        readonly object abortedLock = new object();

        public string ServiceRoot { get { return serviceRoot; } }

        /// <summary>
        /// Constructor for opening an emulated listener socket using
        /// the Yaler reverse HTTP protocol.
        /// Preconditions
        ///     host != null
        ///     host.Length > 0
        ///     port > 0
        ///     port leq 65536
        ///     relayDomain != null
        ///     relayDomain.Length >= 11
        /// </summary>
        /// <param name="host">Internet address or domain name of the
        /// relay server.</param>
        /// <param name="port">Port number of the relay service.</param>
        /// <param name="relayDomain">Yaler relay domain.</param>
        public YalerListener()
        {
            IsOpen = false;
            Host = null;
            Port = 80;
            Domain = null;
        }

        public void Open()
        {
            Contract.Requires(Host != null);
            Contract.Requires(Host.Length > 0);
            Contract.Requires(Port > 0);
            Contract.Requires(Port <= 65535);
            Contract.Requires(Domain != null);
            Contract.Requires(Domain.Length >= 11);
            try
            {
                // TODO test
                Debug.Print("a relay will be used");

                serviceRoot = GetServiceRoot();
                IsOpen = true;
            }
            catch
            {
                Close();
            }
        }

        /// <summary>
        /// Close the emulated listener.
        /// </summary>
        public void Close()
        {
            lock (abortedLock)
            {
                aborted = true;
            }
            try
            {
                if (listener != null)
                {
                    listener.Close();
                    listener = null;
                    serviceRoot = null;
                }
            }
            catch { }
        }

        void Find(string pattern, Socket s, out bool found)
        {
            int[] x = new int[pattern.Length];
            byte[] b = new byte[1];
            int i = 0, j = 0, t = 0;
            do
            {
                found = true;
                for (int k = 0; (k != pattern.Length) && found; k++)
                {
                    if (i + k == j)
                    {
                        int n = s.Receive(b);
                        x[j % x.Length] = n != 0 ? b[0] : -1;
                        j = j + 1;
                    }
                    t = x[(i + k) % x.Length];
                    found = pattern[k] == t;
                }
                i = i + 1;
            } while (!found && (t != -1));
        }

        void FindLocation(Socket s, out string host, out int port)
        {
            host = null;
            port = 80;
            bool found;
            Find("\r\nLocation: http://", s, out found);
            if (found)
            {
                char[] stringChars = new char[40];
                var stringIndex = 0;
                byte[] x = new byte[1];
                int n = s.Receive(x);
                while ((n != 0) && (x[0] != ':') && (x[0] != '/'))
                {
                    stringChars[stringIndex] = (char)x[0];
                    stringIndex = stringIndex + 1;
                    n = s.Receive(x);
                }
                if (x[0] == ':')
                {
                    port = 0;
                    n = s.Receive(x);
                    while ((n != 0) && (x[0] != '/'))
                    {
                        port = 10 * port + x[0] - '0';
                        n = s.Receive(x);
                    }
                }
                host = new string(stringChars, 0, stringIndex);
            }
        }

        /// <summary>
        /// Register this device at the relay service and return a socket
        /// for upcoming requests (and their responses).
        /// </summary>
        /// <returns>Socket on which a request can be awaited.</returns>
        public Stream Accept()
        {
            bool a;
            lock (abortedLock)
            {
                a = aborted;
            }
            if (a)
            {
                throw new InvalidOperationException();
            }
            else
            {
                string host = Host;
                int port = Port;
                Socket s;
                bool acceptable;    // end of headers found
                int[] x = new int[3];
                byte[] b = new byte[1];

                // test
                Debug.Print("");
                Debug.Print("use relay at " + host);

                do
                {
                    // look up relay host's domain name,
                    // to find IP address(es)
                    IPHostEntry hostEntry = Dns.GetHostEntry(host);
                    // SocketException in GetHostEntry would
                    // indicate that host was not found
                    Debug.Print(host + " is resolved");
                    // extract a returned address
                    IPAddress hostAddress = hostEntry.AddressList[0];
                    IPEndPoint remoteEndPoint = new IPEndPoint(hostAddress,
                        port);

                    listener = new Socket(
                        AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                    listener.SetSocketOption(SocketOptionLevel.Tcp,
                        SocketOptionName.NoDelay, true);
                    listener.Connect(remoteEndPoint);
                    Debug.Print("relay appears connected");
                    s = listener;
                    do
                    {
                        s.Send(Encoding.UTF8.GetBytes(
                            "POST /" + Domain + " HTTP/1.1\r\n" +
                            "Upgrade: PTTH/1.0\r\n" +
                            "Connection: Upgrade\r\n" +
                            "Host: " + host + "\r\n\r\n"));
                        Debug.Print("device is registered with Yaler, waiting for a request (POST sent)...");
                        for (int j = 0; j != 12; j = j + 1)
                        {
                            int n = s.Receive(b);
                            x[j % 3] = n != 0 ? b[0] : -1;
                        }
                        Debug.Print("12 bytes received");
                        if ((x[0] == '3') && (x[1] == '0') && (x[2] == '7'))
                        {
                            Debug.Print("redirected...");
                            FindLocation(s, out host, out port);
                        }
                        Find("\r\n\r\n", s, out acceptable);
                        Debug.Assert(acceptable);  // test
                    } while (acceptable && ((x[0] == '2') && (x[1] == '0') && (x[2] == '4')));
                    if (!acceptable || (x[0] != '1') || (x[1] != '0') || (x[2] != '1'))
                    {
                        // test
                        if (acceptable)
                        {
                            Debug.Print("bad relay status code: ");
                            Debug.Print(((char)x[0]).ToString());
                            Debug.Print(((char)x[1]).ToString());
                            Debug.Print(((char)x[2]).ToString());
                        }

                        s.Close();
                        s = null;
                    }
                } while (acceptable && ((x[0] == '3') && (x[1] == '0') && (x[2] == '7')));
                listener = null;

                // TODO test
                if (s == null) { Debug.Print("no relay connection possible"); }

                return new NetworkStream(s);
            }
        }

        string GetServiceRoot()
        {
            NetworkInterface[] nis =
                NetworkInterface.GetAllNetworkInterfaces();
            // we assume that a network interface is available
            Contract.Assert(nis[0] != null);

            var x = new char[17];
            var i = 0;
            byte[] a = nis[0].PhysicalAddress;
            const string hex = "0123456789ABCDEF";
            for (var j = 0; j != 6; j = j + 1)
            {
                x[i] = (char)(hex[(a[j] & 0xF0) >> 4]);
                i = i + 1;
                x[i] = (char)(hex[a[j] & 0x0F]);
                i = i + 1;
                if (i != 17)
                {
                    x[i] = '-';
                    i = i + 1;
                }
            }
            string mac = new string(x);

            Debug.Print("DHCP enabled: " + nis[0].IsDhcpEnabled);
            Debug.Print("MAC address: " + mac);
            string localHost = nis[0].IPAddress;
            Debug.Print("Device address: " + localHost);
            Debug.Print("Gateway address: " + nis[0].GatewayAddress);
            if (nis[0].DnsAddresses.Length > 0)
            {
                Debug.Print("Primary DNS address: " +
                    nis[0].DnsAddresses[0]);
            }
            string uri = "http://" + Host;
            if (Port != 80)
            {
                uri = uri + ":" + Port;
            }
            uri = uri + "/" + Domain;
            Debug.Print("Base Uri: " + uri + "/");
            return uri;
        }
    }
}
