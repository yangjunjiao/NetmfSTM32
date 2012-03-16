using System;
using Microsoft.SPOT;

namespace MFConsoleApplication1
{
    using System.IO.Ports;
    using System.Text;

    class SerialPortExt : SerialPort
    {
        public SerialPortExt(string portName)
            : base(portName)
        {
        }

        public SerialPortExt(string portName, int baudRate)
            : base(portName, baudRate)
        {
        }

        public SerialPortExt(string portName, int baudRate, Parity parity)
            : base(portName, baudRate, parity)
        {
        }

        public SerialPortExt(string portName, int baudRate, Parity parity, int dataBits)
            : base(portName, baudRate, parity, dataBits)
        {
        }

        public SerialPortExt(string portName, int baudRate, Parity parity, int dataBits, StopBits stopBits)
            : base(portName, baudRate, parity, dataBits, stopBits)
        {
        }

        public void Write (string valueString)
        {
            // convert the string to bytes
                byte[] buffer = Encoding.UTF8.GetBytes(valueString);
                // send the bytes on the serial port
                Write(buffer, 0, buffer.Length);
        }
        
        public void WriteLine (string valueString)
        {
            // convert the string to bytes
                byte[] buffer = Encoding.UTF8.GetBytes(valueString + "\r\n");
                // send the bytes on the serial port
                Write(buffer, 0, buffer.Length);
        }
    }
}
