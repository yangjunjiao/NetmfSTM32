using System;
using System.Threading;
using Gsiot.CosmClient;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using Mountaineer.NETMF.Hardware;

public class HelloCosm
{
    public static void Main()
    {
        const string apiKey = "<insert your API key here>";
        const string feedId = "<insert your feed ID here>";
        const int samplingPeriod = 20000;   // 20 seconds

        var voltagePort = new AnalogInput(AnalogChannel.ANALOG_Socket6_Pin3);
        voltagePort.Scale = 3.3;                    // convert to Volt

        while (true)
        {
            WaitUntilNextPeriod(samplingPeriod);
            double value = voltagePort.Read();
            string sample = "voltage," + value.ToString("f");
            Debug.Print("new message: " + sample);
            CosmClient.Send(apiKey, feedId, sample);
        }
    }

    static void WaitUntilNextPeriod(int period)
    {
        long now = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
        var offset = (int)(now % period);
        int delay = period - offset;
        Debug.Print("sleep for " + delay + " ms\r\n");
        Thread.Sleep(delay);
    }
}
