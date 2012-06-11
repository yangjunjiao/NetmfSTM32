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

using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Gsiot.Contracts;
using Microsoft.SPOT;
using Microsoft.SPOT.Net.NetworkInformation;

namespace Gsiot.Listeners.Tcp
{
    public sealed class TcpListener : Listener
    {
        public bool IsOpen { get; private set; }
        public int Port { get; set; }
        public int Backlog { get; set; }

        Socket listener;
        string serviceRoot;

        public TcpListener()
        {
            IsOpen = false;
            Port = 0;
            Backlog = 0;
        }

        public void Open()
        {
            Contract.Requires(Port > 0);
            Contract.Requires(Port <= 65535);
            Contract.Requires(Backlog >= 0);
            if (!IsOpen)
            {
                Contract.Assert(listener == null);
                // only necessary for lwIP stack?
                IPAddress myAdr = IPAddress.GetDefaultLocalAddress();
                while (myAdr == IPAddress.Any)
                {
                    Thread.Sleep(1000);
                    myAdr = IPAddress.GetDefaultLocalAddress();
                }
                try
                {
                    listener = new Socket(AddressFamily.InterNetwork,
                        SocketType.Stream, ProtocolType.Tcp);
                    listener.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, true);
                    listener.Bind(new IPEndPoint(IPAddress.Any, Port));
                    listener.Listen(Backlog);
                    serviceRoot = GetServiceRoot();
                    IsOpen = true;
                }
                catch (SocketException e)
                {
                    Debug.Print("exception in Tcp.Open:\r\n" + e);
                    if (listener != null) { listener.Close(); }
                    listener = null;
                    throw new IOException("socket error " + e.ErrorCode, e);
                }
            }
        }

        public void Close()
        {
            if (IsOpen)
            {
                IsOpen = false;
                Contract.Assert(listener != null);
                listener.Close();
                listener = null;
                serviceRoot = null;
            }
        }

        public string ServiceRoot
        {
            get { Contract.Requires(serviceRoot != null); return serviceRoot; }
        }

        public Stream Accept()
        {
            try
            {
                Socket s = listener.Accept();
                // Socket.Accept may throw an exception: SocketException,
                // ObjectDisposedException, or InvalidOperationException
                return new NetworkStream(s);
            }
            catch (SocketException e)     // this may happen, treat as expected error
            {
                throw new IOException("socket error " + e.ErrorCode, e);
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
            string uri = "http://" + localHost;
            if (Port != 80)
            {
                uri = uri + ":" + Port;
            }
            return uri;
        }
    }
}
