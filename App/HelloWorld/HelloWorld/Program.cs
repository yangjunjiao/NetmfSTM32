using System;
using Microsoft.SPOT;
using System.Threading;

namespace HelloWorld
{
    public class Program
    {
        public static void Main()
        {
            Debug.Print(
                Resources.GetString(Resources.StringResources.String1));
            while (true)
            {
                Thread.Sleep(100);
            }
                
        }

    }
}
