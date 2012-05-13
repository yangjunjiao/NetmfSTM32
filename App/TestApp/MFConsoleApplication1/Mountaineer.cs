using System;
using Microsoft.SPOT;

namespace MFTestApp
{
    using System.Net;
    using System.Net.Sockets;
    using System.Threading;

    //using MFCommonExtension.Strings;

    using Microsoft.SPOT.Net.NetworkInformation;

    public class Mountaineer
    {
        public static void Run()
        {
            //Debug.Print(StringUtil.Format("APP started {0}.", DateTime.Now));

            NetworkInterface[] allNetworkInterfaces = NetworkInterface.GetAllNetworkInterfaces();
            foreach (var networkInterface in allNetworkInterfaces)
            {
                if (networkInterface.NetworkInterfaceType == NetworkInterfaceType.Ethernet)
                {
                    //networkInterface.PhysicalAddress = new byte[] { 0x00, 0xFF, 0xC3, 0x1E, 0x77, 0x01 };
                    //networkInterface.EnableStaticIP("192.168.0.44", "255.255.255.0", "192.168.0.1");

                    networkInterface.EnableDhcp();
                    Thread.Sleep(2000);
                    Debug.Print(networkInterface.NetworkInterfaceType.ToString());
                    Debug.Print(networkInterface.IPAddress);
                }
            }
            

            Debug.Print("Network interface configured.");
           // Debug.Print(StringUtil.Format("IP {0}, NM {1}, GW {2}.", eth.IPAddress, eth.SubnetMask, eth.GatewayAddress));

            var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            //IPEndPoint relayEndpoint = new IPEndPoint(IPAddress.Parse("78.46.63.147"), 9090);

            IPEndPoint relayEndpoint = new IPEndPoint(IPAddress.Parse("192.168.0.1"), 80);

            try
            {
                socket.Connect(relayEndpoint);
            }
            catch (Exception ex)
            {
                //Debug.Print(StringUtil.Format("Connect error {0}. ST {1}.", ex.Message, ex.StackTrace));
                Debug.Print(ex.Message);
            }


            while (true)
            {
                try
                {
                    if (socket.Poll(100, SelectMode.SelectRead))
                    {
                        Debug.Print("Socket connected.");
                    }
                }
                catch (Exception ex)
                {
                    Debug.Print("Socket not connected.");
                }


                Thread.Sleep(500);
            }

            
        }

    }
}