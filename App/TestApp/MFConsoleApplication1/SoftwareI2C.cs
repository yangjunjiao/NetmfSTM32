using System;
using Microsoft.SPOT;

namespace MFConsoleApplication1
{
    using Microsoft.SPOT.Hardware;

    class TimeOutException : ApplicationException
    {
        public TimeOutException()
            : base("Timeout Error")
        {
        }

        public TimeOutException(string message)
            : base(message)
        {
        }
    }

    class SoftwareI2C
    {
        TristatePort scl, sda;
        int i2cSpeed = 100;              // This variable isn't used ... Remove it?
        int timeOut = 10;                // Clock stretching time out interval in msec.
        long timeOutTicks = 0;           // Number of ticks in time out interval.

        bool start;

        public SoftwareI2C(Cpu.Pin SCL, Cpu.Pin SDA, int clock)
        {
            start = false;
            timeOutTicks = (long)timeOut * TimeSpan.TicksPerMillisecond;
            //
            scl = new TristatePort(SCL, false, false, Port.ResistorMode.PullUp);
            sda = new TristatePort(SDA, false, false, Port.ResistorMode.PullUp);
            // scl and sda default to input, the next 2 lines make this explicit.
            MakePinInput(scl);
            MakePinInput(sda);
            //i2cSpeed = clock/ 1111????????????????
        }

        static void MakePinOutput(TristatePort port)
        {
            if (!port.Active)
                port.Active = true;
        }

        static void MakePinInput(TristatePort port)
        {
            if (port.Active)
                port.Active = false;
        }

        //
        // I left this routine in place, but didn't add any delay code.  With
        //  no additional delay beyond the overhead associated with a call to
        //  I2CDELAY, the serial clock frequency is approximately 1.5 KHz.
        //
        void I2CDELAY(int delay)
        {
            //add code for delay
        }

        void ClearSCL()
        {
            MakePinOutput(scl);
        }

        void ClearSDA()
        {
            MakePinOutput(sda);
        }

        bool ReadSCL()
        {
            MakePinInput(scl);
            return scl.Read();
        }

        bool ReadSDA()
        {
            MakePinInput(sda);
            return sda.Read();
        }

        bool ReadBit()
        {
            // "ReadSDA" makes SDA an input - processor lets go of pin and internal
            //  pull-up resistor makes it high.  Now slave can drive the pin.
            ReadSDA();

            I2CDELAY(i2cSpeed);

            // Clock stretching - Makes SCL an input and pull-up resistor makes
            //  the pin high.  Slave device can pull SCL low to extend clock cycle.
            long endStretch = Utility.GetMachineTime().Ticks + timeOutTicks;
            while (!ReadSCL())
            {
                // How long have we been stuck in the while loop?
                if (Utility.GetMachineTime().Ticks >= endStretch)
                    throw new TimeOutException();           // Too long, so bail out by throwing an exception.
            }

            // At this point, SCL is high and SDA is valid - so read the bit.
            bool bit = sda.Read();

            I2CDELAY(i2cSpeed);

            ClearSCL();     // Pull the serial clock line low ...

            return bit;     //  and return.
        }

        bool WriteBit(bool bit)
        {
            if (bit)
            {
                ReadSDA();      // Make SDA an input ... so pin is pulled up.
            }
            else
            {
                ClearSDA();     // Make SDA an output ... so pin is pulled low.
            }

            I2CDELAY(i2cSpeed);
            // Clock stretching - Makes SCL an input and pull-up resistor makes
            //  the pin high.  Slave device can pull SCL low to extend clock cycle.
            long endStretch = Utility.GetMachineTime().Ticks + timeOutTicks;
            while (!ReadSCL())
            {
                // How long have we been stuck in the while loop?
                if (Utility.GetMachineTime().Ticks >= endStretch)
                    throw new TimeOutException();           // Too long, so bail out by throwing an exception.
            }
            // SCL is high and SDA is valid ...
            //  Check that nobody else is driving SDA
            if (bit && !ReadSDA())
            {
                return false;       // Lost arbitration
            }

            I2CDELAY(i2cSpeed);
            ClearSCL();

            return true;           // Success!
        }

        bool SendStartCondition()
        {
            if (start)
            {
                // set SDA to 1
                ReadSDA();
                I2CDELAY(i2cSpeed);
                //
                // Clock stretching - Makes SCL an input and pull-up resistor makes
                //  the pin high.  Slave device can pull SCL low to extend clock cycle.
                long endStretch = Utility.GetMachineTime().Ticks + timeOutTicks;
                while (!ReadSCL())
                {
                    // How long have we been stuck in the while loop?
                    if (Utility.GetMachineTime().Ticks >= endStretch)
                        throw new TimeOutException();           // Too long, so bail out by throwing an exception.
                }
            }

            if (!ReadSDA())
            {
                return false;
            }

            // SCL is high, set SDA from 1 to 0
            ClearSDA();
            I2CDELAY(i2cSpeed);
            ClearSCL();

            start = true;

            return true;
        }

        bool SendStopCondition()
        {
            // set SDA to 0
            ClearSDA();
            I2CDELAY(i2cSpeed);
            //
            // Clock stretching - Makes SCL an input and pull-up resistor makes
            //  the pin high.  Slave device can pull SCL low to extend clock cycle.
            long endStretch = Utility.GetMachineTime().Ticks + timeOutTicks;
            while (!ReadSCL())
            {
                // How long have we been stuck in the while loop?
                if (Utility.GetMachineTime().Ticks >= endStretch)
                    throw new TimeOutException();           // Too long, so bail out by throwing an exception.
            }
            //
            // SCL is high, set SDA from 0 to 1
            if (!ReadSDA())
            {
                return false;
            }

            I2CDELAY(i2cSpeed);
            start = false;

            return true;
        }

        /// <summary>
        /// Sends data to the remote device
        /// </summary>
        /// <param name="sendStartCondition">Perform a HIGH to LOW transition of SDA line while the SCL is high.</param>
        /// <param name="sendStopCondition">Perform a LOW to HIGH transition of SDA line while the SCL is high.</param>
        /// <param name="byteToSend">Byte to transmit</param>
        /// <returns></returns>
        public bool Transmit(bool sendStartCondition, bool sendStopCondition, byte byteToSend)
        {
            if (sendStartCondition)
            {
                try
                {
                    SendStartCondition();
                }
                catch (TimeOutException e)
                {
                    throw new TimeOutException(e.Message);
                }
            }
            for (int bit = 0; bit < 8; bit++)
            {
                try
                {
                    WriteBit((byteToSend & 0x80) != 0);
                }
                catch (TimeOutException e)
                {
                    throw new TimeOutException(e.Message);
                }

                byteToSend <<= 1;
            }
            bool nack;
            try
            {
                nack = ReadBit();
            }
            catch (TimeOutException e)
            {
                throw new TimeOutException(e.Message);
            }
            //
            if (sendStopCondition)
            {
                try
                {
                    SendStopCondition();
                }
                catch (TimeOutException e)
                {
                    throw new TimeOutException(e.Message);
                }
            }
            // Return value is "true" for NAK
            //  "false" for ACK.
            return nack;
        }

        /// <summary>
        /// Receive data from remote device.
        /// </summary>
        /// <param name="acknowledgeBit">
        /// Each device when addressed to has to generate an acknowledge signal after the reception of each byte.
        /// The master generates an extra clock pulse which is associated with the <c>acknowledgeBit</c>.
        /// The device that acknowledges pulls down the SDA line during the acknowledge clock pulse.
        /// </param>
        /// <param name="sendStopCondition">Perform a LOW to HIGH transition of SDA line while the SCL is high.</param>
        /// <returns></returns>
        ///
        //  18 April 2011 - Changed parameter "acknowledgeBit" to "sendAcknowledgeBit" to make it consistent with
        //   "sendStopCondition" and parameters of the "Transmit" routine.
        public byte Receive(bool sendAcknowledgeBit, bool sendStopCondition)
        {
            byte d = 0;
            bool b;

            for (int bit = 0; bit < 8; bit++)
            {
                d <<= 1;

                try
                {
                    b = ReadBit();
                }
                catch (TimeOutException e)
                {
                    throw new TimeOutException(e.Message);
                }
                if (b)
                    d |= 1;
            }
            //
            try
            {
                WriteBit(!sendAcknowledgeBit);
            }
            catch (TimeOutException e)
            {
                throw new TimeOutException(e.Message);
            }
            //
            if (sendStopCondition)
            {
                try
                {
                    SendStopCondition();
                }
                catch (TimeOutException e)
                {
                    throw new TimeOutException(e.Message);
                }
            }
            //
            return d;
        }

    }
}
