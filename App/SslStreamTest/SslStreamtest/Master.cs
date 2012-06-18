////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Net.Security;
//using Microsoft.SPOT.Platform.Tests;
//using System.Security.Cryptography.X509Certificates;


namespace Microsoft.SPOT.Platform.Tests
{
    public class Master_SslStream
    {
        public static void Main()
        {
            //Set the system time to be valid relative to the certificate expiration dates.
            //DateTime systemTime = new DateTime(2008, 01, 01, 01, 01, 01);

            //Debug.Print("Setting System Time to: " + systemTime.ToString());
            //Hardware.Utility.SetLocalTime(systemTime);

            /*string[] args = { 
                "X509CertificateTests",
                "SslStreamTests",
                //"CertificateStoreTests", 
            };

            MFTestRunner runner = new MFTestRunner(args);*/
            TestMethod1();
        }


        private static bool TestMethod1()
        {
           // MFTestResults testResult = MFTestResults.Fail;
            bool testResult = false;

            Debug.Print("Create the client and server sockets");

            SslServer sslServer = new SslServer();
            SslClient sslClient = new SslClient();

            try
            {
                // Set up the server here
                Debug.Print("Create server thread");
                sslServer.RunServer();

                // Set up the client here
                sslClient.serverEp = sslServer.serverEp;
                sslClient.RunClient();

                if (sslClient.messageSent == sslClient.messageReceived)
                    testResult =true;
            }
            catch (Exception e)
            {
                Debug.Print("Incorrect Exception caught: " + e.ToString());
            }
            finally
            {
                try
                {
                    sslServer.Close();
                    sslClient.Close();
                }
                catch
                {
                }
            }
            return testResult;
        }

    }
}