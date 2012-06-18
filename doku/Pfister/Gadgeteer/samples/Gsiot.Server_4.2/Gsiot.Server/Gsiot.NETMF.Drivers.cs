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

// These classes provide objects for sensors and actuators that implement
// a common interface pattern: Properties are used to configure an object,
// and Open method checks the properties and created internal objects as
// needed, and HandleGet and HandleSet methods provide a standard way for
// getting new samples from a sensor, or for setting and getting setpoints
// for an actuator.

using Gsiot.Contracts;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace Gsiot.NETMF.Drivers
{
    /// <summary>
    /// Represents a digital input (GPIO pin).
    /// </summary>
    public class DigitalSensor
    {
        Cpu.Pin pin = Cpu.Pin.GPIO_NONE;
        InputPort port;

        /// <summary>
        /// Mandatory property that denotes the pin to be used as input.
        /// Must be set before sensor is opened.
        /// </summary>
        public Cpu.Pin InputPin
        {
            get { return pin; }

            set
            {
                Contract.Requires(port == null);    // sensor is not open
                pin = value;
            }
        }

        /// <summary>
        /// To be called after setting the properties of the object.
        /// If not called explicitly, it is automatically called when
        /// the sensor is used for the first time.
        /// Preconditions
        ///     Sensor is not open
        ///     InputPin is set
        /// Postconditions
        ///     Sensor is open
        /// </summary>
        public void Open()
        {
            Contract.Requires(port == null);        // sensor is not open
            Contract.Requires(InputPin != Cpu.Pin.GPIO_NONE);
            port = new InputPort(InputPin, false,
                Port.ResistorMode.Disabled);
        }

        /// <summary>
        /// Returns a new sample.
        /// If the sensor is not open, Open is called first.
        /// Postconditions
        ///     (Result == null) || (Result is bool)
        /// </summary>
        public object HandleGet()
        {
            if (port == null) { Open(); }
            return port.Read();     // bool converted to object
        }
    }

    /// <summary>
    /// Represents a digital output (GPIO pin).
    /// </summary>
    public class DigitalActuator
    {
        Cpu.Pin pin = Cpu.Pin.GPIO_NONE;
        bool actualState;
        OutputPort port;

        /// <summary>
        /// Mandatory property that denotes the pin to be used as output.
        /// Must be set before actuator is opened.
        /// </summary>
        public Cpu.Pin OutputPin
        {
            get { return pin; }

            set
            {
                Contract.Requires(port == null);    // actuator is not open
                pin = value;
            }
        }

        /// <summary>
        /// Optional property that denotes the initial state of the output.
        /// Default: false
        /// Preconditions
        ///      Actuator is not open
        /// </summary>
        public bool InitialState
        {
            get { return actualState; }

            set
            {
                Contract.Requires(port == null);    // actuator is not open
                actualState = value;
            }
        }

        /// <summary>
        /// To be called after setting the properties of the object.
        /// If not called explicitly, it is automatically called when
        /// the sensor is used for the first time.
        /// Preconditions
        ///     Actuator is not open
        ///     OutputPin is set
        /// Postconditions
        ///     Actuator is open
        /// </summary>
        public void Open()
        {
            Contract.Requires(port == null);        // actuator is not open
            Contract.Requires(OutputPin != Cpu.Pin.GPIO_NONE);
            port = new OutputPort(OutputPin, actualState);
        }

        /// <summary>
        /// Sets a new setpoint.
        /// If the actuator is not open, Open is called first.
        /// Preconditions
        ///     setpoint != null
        ///     setpoint is bool
        /// </summary>
        public void HandlePut(object setpoint)
        {
            Contract.Requires(setpoint != null);
            Contract.Requires(setpoint is bool);
            if (port == null) { Open(); }
            actualState = (bool)setpoint;
            port.Write(actualState);
        }

        /// <summary>
        /// Returns most recent setpoint.
        /// If none has been set yet, the initial state is returned.
        /// If the actuator is not open, Open is called first.
        /// Postconditions
        ///     (Result == null) || (Result is bool)
        /// </summary>
        public object HandleGet()
        {
            if (port == null) { Open(); }
            return actualState;
        }
    }

    /// <summary>
    /// Represents an analog input.
    /// If the MinValue or MaxValue property is set, then the
    /// input value is linearly scaled to this range and returned
    /// as a double value.
    /// Otherwise the raw integer value is returned as an int value.
    /// </summary>
    public class AnalogSensor   // TODO test Resolution property, documentation
    {
        /// <summary>
        /// Mandatory property that denotes the channel to be used as input.
        /// Must be set before sensor is opened.
        /// </summary>
        public Cpu.AnalogChannel Channel { get; set; }

        public double MinValue { get; set; }

        public double MaxValue { get; set; }

        public AnalogSensor()
        {
            MinValue = 0.0;
            MaxValue = 3.3;
        }

        AnalogInput port;

        /// <summary>
        /// To be called after setting the properties of the object.
        /// If not called explicitly, it is automatically called when
        /// the sensor is used for the first time.
        /// Preconditions
        ///     Sensor is not open
        ///     Channel is set
        ///     if MinValue or MaxValue is set: MinValue lessThan MaxValue
        /// Postconditions
        ///     Sensor is open
        /// </summary>
        public void Open()
        {
            Contract.Requires(port == null);        // sensor is not open
            Contract.Requires(Channel >= 0);
            Contract.Requires(MinValue < MaxValue);
            port = new AnalogInput(Channel);
            port.Offset = MinValue;
            port.Scale = MaxValue - MinValue;
        }

        /// <summary>
        /// Returns a new sample.
        /// If the sensor is not open, Open is called first.
        /// Postconditions
        ///     (Result == null) || (Result is int) || (Result is double)
        /// </summary>
        public object HandleGet()
        {
            if (port == null) { Open(); }
            double value = port.Read();

Debug.Print("raw ain value: " + port.ReadRaw());


            //Debug.Print("get (already scaled and offset) value " + value);
            return value;
        }
    }

    /// <summary>
    /// Represents a PWM output, which can often be used instead of a
    /// true analog output.
    /// If the MinValue and MaxValue properties are set, then the
    /// input value is linearly scaled from this range.
    /// Otherwise [0.0 .. 3.3] is used as default range.
    /// </summary>
    public class PWMActuator
    {
        /// <summary>
        /// Mandatory property that denotes the channel to be used as input.
        /// Must be set before actuator is opened.
        /// </summary>
        public int Channel { get; set; }

        /// <summary>
        /// Mandatory property that denotes the period of the PWM in
        /// microseconds.
        /// Must be set before actuator is opened.
        /// </summary>
        public int Period { get; set; }

        /// <summary>
        /// Optional property that denotes the minimal input value that
        /// is linearly scaled to a zero duration.
        /// Default: 0.0
        /// </summary>
        public double MinValue { get; set; }

        /// <summary>
        /// Optional property that denotes the maximal input value that
        /// is linearly scaled to a full duration.
        /// Default: 3.3
        /// </summary>
        public double MaxValue { get; set; }

        public PWMActuator()
        {
            MinValue = 0.0;
            MaxValue = 3.3;
        }

        double minValue;
        double maxValue;
        PWM port;
        object lastSetpoint = null;

        /// <summary>
        /// To be called after setting the properties of the object.
        /// If not called explicitly, it is automatically called when
        /// the actuator is used for the first time.
        /// Preconditions
        ///     Actuator is not open
        ///     Channel is set
        ///     Period is set
        ///     if MinValue or MaxValue is set: MinValue lessThan MaxValue
        /// Postconditions
        ///     Actuator is open
        /// </summary>
        public void Open()
        {
            Contract.Requires(port == null);        // actuator is not open
            Contract.Requires(Channel >= 0);
            Contract.Requires(Period > 0);
            Contract.Requires(MinValue < MaxValue);
            minValue = MinValue;
            maxValue = MaxValue;
            port = new PWM((Cpu.PWMChannel)Channel, (uint)Period, 0, PWM.ScaleFactor.Microseconds, false);
            port.Start();
        }

        /// <summary>
        /// Sets a new setpoint.
        /// If the actuator is not open, Open is called first.
        /// Preconditions
        ///     setpoint != null
        ///     setpoint is double
        /// </summary>
        public void HandlePut(object setpoint)
        {
            Contract.Requires(setpoint != null);
            Contract.Requires(setpoint is double);
            double x = (double)setpoint;
            if (x < MinValue) { x = MinValue; }
            else if (x > MaxValue) { x = MaxValue; }
            if (port == null) { Open(); }
            int value = (int)(((x - MinValue) * Period) / (MaxValue - MinValue));
            //Debug.Print("set value " + value + " -> " + (uint)value);
            port.Duration = (uint)value;
            lastSetpoint = setpoint;
        }

        /// <summary>
        /// Returns most recent setpoint.
        /// If none has been set yet, the initial state is returned.
        /// If the actuator is not open, Open is called first.
        /// Postconditions
        ///     (Result == null) || (Result is double)
        /// </summary>
        public object HandleGet()
        {
            if (port == null) { Open(); }
            return lastSetpoint;
        }
    }
}
