using System;
using Microsoft.SPOT;

namespace MFConsoleApplication1
{
    using System.Threading;

    using Microsoft.SPOT.Hardware;

    
    /// <summary>
    /// </summary>
    public class Ds1624
    {
        private I2CDevice i2cDevice;

        private bool runThread = false;

        private Thread thread;

        private double lastTemperature;

        public delegate void TemperatureEventHandler (double temperature);

        public event TemperatureEventHandler TemperatureEvent = delegate { };

        public double LastTemperature
        {
            get
            {
                return this.lastTemperature;
            }
        }

        public void Init()
        {
            I2CDevice.Configuration configuration = new I2CDevice.Configuration(72, 400);
            this.i2cDevice = new I2CDevice(configuration);
            this.lastTemperature = 0.0;
        }

        public void StartConversion()
        {
            if (this.runThread == false)
            {
                this.thread = new Thread(this.ds1624Thread);
                this.runThread = true;
                this.thread.Start();
            }            
        }


        private void ds1624Thread()
        {
            I2CDevice.I2CTransaction[] cmdStartConversion = new I2CDevice.I2CTransaction[] { I2CDevice.CreateWriteTransaction(new byte[]  {0xEE}) };
            I2CDevice.I2CTransaction[] cmdStartRead = new I2CDevice.I2CTransaction[] { I2CDevice.CreateWriteTransaction(new byte[] { 0xAA }) };
            byte[] temperatureData = new byte[2];
            I2CDevice.I2CTransaction[] cmdRead = new I2CDevice.I2CTransaction[] { I2CDevice.CreateReadTransaction(temperatureData) };

            while (runThread == true)
            {
                if (i2cDevice.Execute(cmdStartConversion, 100) == 0)
                {
                    throw new Exception("i2c");    
                }

                Thread.Sleep(1000);
                if (i2cDevice.Execute(cmdStartRead, 100) == 0)
                {
                    throw new Exception("i2c");
                }

                if (i2cDevice.Execute(cmdRead, 100) < 2)
                {
                    throw new Exception("i2c");
                }
                else
                {
                    this.lastTemperature = this.ConvertTemperature(temperatureData); 
                    this.TemperatureEvent(this.LastTemperature);
                }

            }
        }

        private double ConvertTemperature(byte[] temperature)
        {
            //TODO negative temperatures
            return temperature[0] + (temperature[1] >> 3) * 0.03125;
        }
       
    }
}
