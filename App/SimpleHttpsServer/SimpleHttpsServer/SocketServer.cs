////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace Util
{
    using System;
    using System.Net;
    using System.Net.Sockets;
    using System.Text;
    using System.Threading;
    using Microsoft.SPOT;
    //using Microsoft.SPOT.Net.Security;

    /// <summary>
    /// This is a simple network server.  Given a request, it returns an HTML 
    /// document.  The same document is returned for all requests and no parsing of 
    /// the request is done.
    /// <para>
    /// If you are running this application in the emulator, you can view the 
    /// resulting page in a Web browser by entering the URL http://127.0.0.1:12000/ 
    /// while the server is running.
    /// </para>
    /// </summary>
     class SocketServer
    {
        private IHtmlDataProvider htmlDataProvider;
        public  SocketServer()
        {
            new Thread(this.Run).Start();
            htmlDataProvider = null;
        }

        public SocketServer(IHtmlDataProvider htmlDataProvider)
        {
            new Thread(this.Run).Start();
            this.htmlDataProvider = htmlDataProvider;
        }

        private  void Run()
        {
            const Int32 c_port = 80;

            // Create a socket, bind it to the server's port, and listen for client 
            // connections.
            Socket server = new Socket(AddressFamily.InterNetwork,
                SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, c_port);
            server.Bind(localEndPoint);
            server.Listen(1);



            while (true)
            {
                try
                {
                    // Wait for a client to connect.
                    Socket clientSocket = server.Accept();

                    // Process the client request.  true means asynchronous.
                    new ProcessClientRequest(clientSocket, htmlDataProvider, false);
                    //Debug.Print(localEndPoint.Address.GetAddressBytes().ToString());
                }
                catch (Exception ex)
                {
                    Debug.Print(ex.Message);
                    Thread.Sleep(500);
                    
                }
            }
        }
    }

    /// <summary>
    /// Processes a client request.
    /// </summary>
    internal sealed class ProcessClientRequest
    {
        private Socket m_clientSocket;

        private IHtmlDataProvider m_htmlDataProvider;

        /// <summary>
        /// The constructor calls another method to handle the request, but can 
        /// optionally do so in a new thread.
        /// </summary>
        /// <param name="clientSocket"></param>
        /// <param name="asynchronously"></param>
        public ProcessClientRequest(Socket clientSocket, IHtmlDataProvider htmlDataProvider, Boolean asynchronously)
        {
            m_clientSocket = clientSocket;
            m_htmlDataProvider = htmlDataProvider;
            if (asynchronously)
                // Spawn a new thread to handle the request.
                new Thread(ProcessRequest).Start();
            else ProcessRequest();
        }

        /// <summary>
        /// Processes the request.
        /// </summary>
        private void ProcessRequest()
        {
            const Int32 c_microsecondsPerSecond = 1000000;

            // 'using' ensures that the client's socket gets closed.
            using (m_clientSocket)
            {
                // Wait for the client request to start to arrive.
                Byte[] buffer = new Byte[1024];
                if (m_clientSocket.Poll(5 * c_microsecondsPerSecond,
                    SelectMode.SelectRead))
                {
                    // If 0 bytes in buffer, then the connection has been closed, 
                    // reset, or terminated.
                    if (m_clientSocket.Available == 0)
                        return;

                    // Read the first chunk of the request (we don't actually do 
                    // anything with it).
                    Int32 bytesRead = m_clientSocket.Receive(buffer,
                        m_clientSocket.Available, SocketFlags.None);

                    String bodyText = "";
                    if (m_htmlDataProvider != null)
                    {
                        bodyText = m_htmlDataProvider.GetData();
                    }
                    else
                    {
                        bodyText = "<bold><a href=\"http://www.microsoft.com/netmf/\">Learn more about the .NET Micro Framework by clicking here</a></bold>";
                    }
                    // Return a static HTML document to the client.
                    String s =
                        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><head><title>.NET Micro Framework Web Server</title></head>" +
                       "<body>"+
                        bodyText +
                        "</body></html>";
                    m_clientSocket.Send(Encoding.UTF8.GetBytes(s));
                }
            }
        }
    }
}
