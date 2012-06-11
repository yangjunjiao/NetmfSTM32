using System;
using Microsoft.SPOT;
using Util;
using System.Threading;

namespace SimpleWebServer
{
    public class Program
    {
        public static void Main()
        {
            Thread.Sleep(200);
            Debug.Print(
                Resources.GetString(Resources.StringResources.String1));
            SocketServer socketServer = new SocketServer(new HtmlHelloWorldData());
            while (true)
            {
                Thread.Sleep(500);
            }
        }

    }
}
