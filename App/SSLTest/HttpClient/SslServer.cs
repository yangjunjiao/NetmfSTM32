using System;
using System.Collections;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using Microsoft.SPOT.Net;
using Microsoft.SPOT.Net.Security;

using Microsoft.SPOT.Net.NetworkInformation;
using System.Security.Cryptography.X509Certificates;


namespace Microsoft.SPOT.Platform.Tests
{
    class SslServer
    {
        //X509 Certificate used for the connection.
        X509Certificate certificate = null;
        X509Certificate [] ca = null;
        SslVerification clientCertificateRequired = SslVerification.NoVerification;
        SslProtocols [] enabledSslProtocols = new SslProtocols [] { SslProtocols.Default };
        
        bool expectedException = false;
        
        //Socket configuration information.
        IPAddress ipAddress = null;
        int port = 11155;

        //The Socket thats used as an Ssl Server.
        Socket sslServer = null;

        public SslServer(X509Certificate certificate, X509Certificate[] ca, SslVerification clientCertificateRequired, SslProtocols[] enabledSslProtocols, bool expectedException)
        {
            this.certificate = certificate;
            this.ca = ca;
            this.clientCertificateRequired = clientCertificateRequired;
            this.enabledSslProtocols = enabledSslProtocols;
            this.expectedException = expectedException;

            //Get the IPV4 address on this machine which is all that the MF devices support.
            IPHostEntry hostEntry = Dns.GetHostEntry("");
            ipAddress = hostEntry.AddressList[0];

            if (ipAddress == null)
                throw new Exception("No IPV4 address found.");

        }

        public bool RunServer()
        {
            //MFTestResults testResult = MFTestResults.Fail;
            bool result = false;

            SslStream sslStream = null;
            Socket client = null;
            try
            {
                if (sslServer == null)
                {
                    IPEndPoint localEndPoint = new IPEndPoint(ipAddress, port);

                    sslServer = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                    //sslServer.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ExclusiveAddressUse, false);
                    //sslServer.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Linger, new byte[] { 0, 0, 0, 0 });


                    sslServer.Bind(localEndPoint);

                    sslServer.Listen(1);
                }

                // Buffer for reading data
                Byte[] bytes = new Byte[2048];
                String data = "";

                // Start Listening for a connection.
                //Log.Comment("Waiting for a connection... on IPAddress: " + localEndPoint.Address.ToString() + " Port: " +localEndPoint.Port.ToString());

                // Perform a blocking call to accept requests.
                // block in listening mode until the desktop app connects.  Then we know we can continue.
                 client = sslServer.Accept();

                //client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Linger, new byte[] { 0, 0, 0, 0 });
                //client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 5000);
                //client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 5000);

                //Log.Comment("Connected!");

                // Get a stream object for reading and writing
                 sslStream = new SslStream(client);          //SK call of SSL_ServerInit in CLR-Code

                sslStream.AuthenticateAsServer(certificate, ca, clientCertificateRequired, enabledSslProtocols);

                sslStream.ReadTimeout = 5000;
                sslStream.WriteTimeout = 5000;
                int i = -1;
               // do
                {
                    // Loop to receive all the data sent by the client.
                    i = sslStream.Read(bytes, 0, bytes.Length);
                   

                    // Translate data bytes to a string.
                    // The encoding used is application specific.
                    string newStr = new string(System.Text.Encoding.UTF8.GetChars(bytes), 0, i);
                    //Log.Comment("Received: " + newStr);

                    data += newStr;

                    //if (data.IndexOf(SslClient.TERMINATOR) != -1)
                    //    break;
                } //while (i != 0);

                // Send back a response.
                //sslStream.Write(bytes, 0, i);
                // Return a static HTML document to the client.
                String s =
                    "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><head><title>.NET Micro Framework Web Server</title></head>" +
                   "<body><bold><a href=\"http://www.microsoft.com/netmf/\">Learn more about the .NET Micro Framework by clicking here</a></bold></body></html>";
                sslStream.Write(Encoding.UTF8.GetBytes(s), 0, Encoding.UTF8.GetBytes(s).Length);
                //Log.Comment("Sent:     " + data);

                Thread.Sleep(200);
                result = true;
            }
            catch (SocketException e)
            {
                if (expectedException)
                    result = true;

                //Log.Comment("SocketException in StartServer(): " + e.ToString());
                //Log.Comment("ErrorCode: " + e.ErrorCode.ToString());
            }
            catch (Exception e)
            {
                if (expectedException)
                    result = true;

                //Log.Comment("Exception in StartServer(): " + e.ToString());
            }
            finally
            {
 
                if (sslStream != null)
                {
                    sslStream.Close();
                    sslStream.Dispose();
                    //sslStream.Flush();
                   
                }
                if (sslServer != null)
                {
                  // sslServer.Close();
                }

               
            }
     return result;
        }


    }

     

}
