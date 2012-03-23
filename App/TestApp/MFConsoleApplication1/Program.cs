namespace MFConsoleApplication1
{
    using System;
    using System.IO.Ports;
    using System.Text;

    using Microsoft.SPOT;
    using Microsoft.SPOT.Hardware;
    using System.Threading;

    using Microsoft.SPOT.Hardware.STM32;
    using Microsoft.SPOT.IO;
    using Microsoft.SPOT.Net.NetworkInformation;

    public class Program
    {
        static bool ledState = false;
        static OutputPort led = new OutputPort(Pins.GPIO_PIN_B_15, ledState);

        public static void Main()
        {
            OutputPort LED = new OutputPort(Pins.GPIO_PIN_B_8, true);


            InterruptPort button = new InterruptPort(Pins.GPIO_PIN_G_8, false, Port.ResistorMode.Disabled, Port.InterruptMode.InterruptEdgeLow);
            button.OnInterrupt += new NativeEventHandler(button_OnInterrupt);
            

            Debug.Print(
                Resources.GetString(Resources.StringResources.String1));

            Debug.Print(Debug.GC(true).ToString());

            ExtendedWeakReferences.Run();
            SerialPortExt uart = new SerialPortExt("COM2", 115200);
            int counter = 0;
            uart.Open();
            uart.WriteLine(Resources.StringResources.String1.ToString());

            Thread threadToggleLed = new Thread(ToggleThread);
            threadToggleLed.Start();

            /*VolumeInfo[] vis = VolumeInfo.GetVolumes();
            foreach (var volumeInfo in vis)
            {
                Debug.Print(volumeInfo.TotalSize.ToString());
            }*/
            
            

            Ds1624 tempSens = new Ds1624();
            tempSens.Init();
            tempSens.TemperatureEvent +=
                delegate(double temperature)
                    {                       
                        uart.WriteLine("temperature: " + temperature.ToString());
                    };
            tempSens.StartConversion();

           // SocketServer socketServer = new SocketServer(new HtmlTempDataProvider(tempSens));


            //new Thread(MyHttpClient.Run).Start();
            
            while (true)
            {
                Thread.Sleep(500);
                LED.Write(false);
                Thread.Sleep(500);
                LED.Write(true);

                uart.WriteLine("Count: " + counter.ToString() + "\r\n");
                counter++;

                NetworkInterface[] allNetworkInterfaces = NetworkInterface.GetAllNetworkInterfaces();
                foreach (var allNetworkInterface in allNetworkInterfaces)
                {
                    uart.WriteLine(allNetworkInterface.NetworkInterfaceType.ToString());
                    uart.WriteLine(allNetworkInterface.IPAddress);
                }
                //AnalogInput analogInput = new AnalogInput(Cpu.AnalogChannel.ANALOG_4);
                //Debug.Print("a: " + analogInput.Read().ToString());
            }
        }



        static void ToggleThread()
        {
            bool ledState = false;
            //OutputPort led2 = new OutputPort(Pins.GPIO_PIN_B_9, false);



         /* SPI.Configuration MyConfig = new SPI.Configuration(
                Pins.GPIO_PIN_B_9, false, 0, 0, false, true, 100, SPI.SPI_module.SPI2);
            SPI MySPI = new SPI(MyConfig);

            byte[] tx_data  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 55};
            byte[] rx_data = new byte[10];
            */


            
            while (true)
            {
                Thread.Sleep(200);
                //Debug.GC(true);
                //Debug.Print(Debug.GC(true).ToString());
                // toggle LED state
                ledState = !ledState;
                //led2.Write(ledState);

             //   MySPI.WriteRead(tx_data, rx_data);


            }
        }

        static void button_OnInterrupt(uint data1, uint data2, DateTime time)
        {
            //throw new NotImplementedException();
            // toggle LED state
            ledState = !ledState;
            led.Write(ledState);
        }

    }
}
