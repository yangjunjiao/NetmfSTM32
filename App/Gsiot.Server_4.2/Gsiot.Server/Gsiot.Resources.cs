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

// The GetHandler and PutHandler delegate types describe the interface
// between sensors/actuators on the one hand, and resource objects on
// the other hand. Any object that provides a method compatible with
// GetHandler can be used as a sensor, and object that provides a
// method compatible with PutHandler can be used as an actuator.

using Gsiot.Contracts;
using Gsiot.Resources;

namespace Gsiot.Server
{
    /// <summary>
    /// Converts an object to an HTTP response message body.
    /// content may be null.
    /// Preconditions
    ///     context != null
    /// 
    /// Postconditions
    ///     "context.ResponseContent contains serialized representation of
    ///      content object"
    ///     "context.ResponseContentType contains the MIME type of the
    ///      representation"
    /// </summary>
    public delegate void Serializer(RequestHandlerContext context,
        object content);

    /// <summary>
    /// Converts an HTTP request message body to an object.
    /// content may be null.
    /// The return value indicates whether conversion was successful.
    /// Preconditions
    ///     context != null
    /// Postconditions
    ///     Result =>
    ///         content != null
    ///         "content contains deserialized representation of
    ///          context.RequestContent"
    ///     !Result =>
    ///         content == null
    ///         "context.RequestContent could not be converted to a
    ///          C# object"
    /// </summary>
    public delegate bool Deserializer(RequestHandlerContext context,
        out object content);

    /// <summary>
    /// Returns a new sample.
    /// The result may be null.
    /// The result type(s) depend on the object.
    /// </summary>
    public delegate object GetHandler();

    /// <summary>
    /// Accepts a new setpoint.
    /// o may be null.
    /// The acceptable type(s) of o depend on the object.
    /// </summary>
    public delegate void PutHandler(object o);

    /// <summary>
    /// Resource for a sensor.
    /// </summary>
    public class MeasuredVariable
    {
        /// <summary>
        /// Mandatory property that must be assigned a method that
        /// produces new samples.
        /// Must be set before measured variable is opened.
        /// </summary>
        public GetHandler FromSensor { get; set; }

        /// <summary>
        /// Optional property that serializes a C# object to a HTTP
        /// response message body.
        /// Default: Gsiot.Server.CSharpRepresentation.Serialize
        /// </summary>
        public Serializer ToHttpResponse { get; set; }

        bool initialized;

        /// <summary>
        /// To be called after setting the properties of the object.
        /// If not called explicitly, it is automatically called when
        /// the measured variable is used for the first time.
        /// Preconditions
        ///     measured variable is not open
        ///     FromSensor is set
        /// Postconditions
        ///     measured variable is open
        /// </summary>
        public void Open()
        {
            Contract.Requires(!initialized);
            Contract.Requires(FromSensor != null);
            if (ToHttpResponse == null)
            {
                ToHttpResponse = CSharpRepresentation.Serialize;
            }
            initialized = true;
        }

        /// <summary>
        /// Interprets an HTTP request and produces an appropriate HTTP
        /// response.
        /// If the measured variable is not open, Open is called first.
        /// Preconditions
        ///     context != null
        /// </summary>
        /// <param name="context">Provides access to the received request,
        /// the server, and to the (empty) response.</param>
        public void HandleRequest(RequestHandlerContext context)
        {
            Contract.Requires(context != null);
            if (!initialized) { Open(); }

            context.ResponseMaxAge = 0;         // no caching (default is -1, i.e., no cache header)
            object sample = FromSensor();
            // sample may be null
            ToHttpResponse(context, sample);
        }
    }

    /// <summary>
    /// Resource for an actuator.
    /// </summary>
    public class ManipulatedVariable
    {
        /// <summary>
        /// Mandatory property that must be assigned a method that
        /// deserializes a HTTP request message body to a C# object.
        /// </summary>
        public Deserializer FromHttpRequest { get; set; }

        /// <summary>
        /// Mandatory property that must be assigned a method that
        /// consumes new setpoints.
        /// Must be set before manipulated variable is opened.
        /// </summary>
        public PutHandler ToActuator { get; set; }

        /// <summary>
        /// Optional property that serializes a C# object to a HTTP
        /// response message body.
        /// Default: Gsiot.Server.CSharpRepresentation.Serialize
        /// </summary>
        public Serializer ToHttpResponse { get; set; }

        bool initialized;
        object lastSetpoint;

        /// <summary>
        /// To be called after setting the properties of the object.
        /// If not called explicitly, it is automatically called when
        /// the manipulated variable is used for the first time.
        /// Preconditions
        ///     manipulated variable is not open
        ///     FromSensor is set
        /// Postconditions
        ///     manipulated variable is open
        ///     // TODO update docu (default for FromHttpRequest)
        /// </summary>
        public void Open()
        {
            Contract.Requires(!initialized);
            Contract.Requires(ToActuator != null);
            if (FromHttpRequest == null)
            {
                FromHttpRequest = CSharpRepresentation.TryDeserializeDouble;
            }
            if (ToHttpResponse == null)
            {
                ToHttpResponse = CSharpRepresentation.Serialize;
            }
            initialized = true;
        }

        /// <summary>
        /// Interprets an HTTP request and produces an appropriate HTTP
        /// response.
        /// If the manipulated variable is not open, Open is called first.
        /// Preconditions
        ///     context != null
        /// </summary>
        /// <param name="context">Provides access to the received request,
        /// the server, and to the (empty) response.</param>
        public void HandleRequest(RequestHandlerContext context)
        {
            Contract.Requires(context != null);
            if (!initialized) { Open(); }
            if (context.RequestMethod == "PUT")
            {
                object setpoint;
                if (FromHttpRequest(context, out setpoint))
                {
                    // setpoint may be null
                    ToActuator(setpoint);
                    lastSetpoint = setpoint;
                    context.ResponseStatusCode = 200;    // OK
                }
                else
                {
                    context.ResponseStatusCode = 400;    // Bad Request
                }
            }
            else if (context.RequestMethod == "GET")
            {
                // lastSetpoint may be null
                ToHttpResponse(context, lastSetpoint);
                context.ResponseStatusCode = 200;    // OK
            }
            else
            {
                context.ResponseStatusCode = 405;    // Method Not Allowed
            }
        }
    }
}
