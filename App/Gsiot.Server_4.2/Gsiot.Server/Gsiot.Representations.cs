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

using Gsiot.Contracts;
using Gsiot.Server;

namespace Gsiot.Resources
{
    /// <summary>
    /// Class that supports serialization of all C# objects, and
    /// deserialization of bool, int and double values.
    /// </summary>
    public static class CSharpRepresentation
    {
        /// <summary>
        /// Converts an object to an HTTP response message body.
        /// content may be null.
        /// If the content is null, the string "null" is produced as
        /// its representation. Otherwise, the object's ToString
        /// method is called for producing the representation.
        /// Preconditions
        ///     context != null
        /// Postconditions
        ///     "context.RequestContent contains serialized representation of
        ///      content object"
        ///     context.ResponseContentType == "text/plain"
        /// </summary>
        public static void Serialize(RequestHandlerContext context,
            object content)
        {
            string s = (content != null) ? content.ToString() : "null";
            context.SetResponse(s, "text/plain");
        }

        /// <summary>
        /// Converts an HTTP request message body to a bool object.
        /// content is not null if conversion was successful.
        /// The return value indicates whether conversion was successful.
        /// Preconditions
        ///     context != null
        /// Postconditions
        ///     Result =>
        ///         content != null
        ///         content is bool
        ///         "content contains deserialized representation of
        ///          context.RequestContent"
        ///     !Result =>
        ///         content == null
        ///         "context.RequestContent could not be converted to a
        ///          C# object"
        /// </summary>
        public static bool TryDeserializeBool(
            RequestHandlerContext context, out object content)
        {
            string s = context.RequestContent;
            if (s == "true") { content = true; return true; }
            else if (s == "false") { content = false; return true; }
            else { content = null; return false; }
        }

        /// <summary>
        /// Converts an HTTP request message body to an int object.
        /// content is not null if conversion was successful.
        /// The return value indicates whether conversion was successful.
        /// Preconditions
        ///     context != null
        /// Postconditions
        ///     Result =>
        ///         content != null
        ///         content is int
        ///         "content contains deserialized representation of
        ///          context.RequestContent"
        ///     !Result =>
        ///         content == null
        ///         "context.RequestContent could not be converted to a
        ///          C# object"
        /// </summary>
        public static bool TryDeserializeInt(RequestHandlerContext context,
            out object content)
        {
            string s = context.RequestContent;
            int x;
            if (Utilities.TryParseUInt32(s, out x))
            {
                content = x;
                return true;
            }
            else
            {
                content = null;
                return false;
            }
        }

        /// <summary>
        /// Converts an HTTP request message body to a double object.
        /// content is not null if conversion was successful.
        /// The return value indicates whether conversion was successful.
        /// Preconditions
        ///     context != null
        /// Postconditions
        ///     Result =>
        ///         content != null
        ///         content is double
        ///         "content contains deserialized representation of
        ///          context.RequestContent"
        ///     !Result =>
        ///         content == null
        ///         "context.RequestContent could not be converted to a
        ///          C# object"
        /// </summary>
        public static bool TryDeserializeDouble(RequestHandlerContext context,
            out object content)
        {
            // TODO remove
            if (context == null) Microsoft.SPOT.Debug.Print("context is null");
            if (context.RequestContent == null) Microsoft.SPOT.Debug.Print("RequestContent is null");

            string s = context.RequestContent;
            double x = 0;
            int decimalDiv = 0;
            content = null;
            foreach (char c in s)
            {
                if ((c == '.') && (decimalDiv == 0)) { decimalDiv = 1; }
                else if ((c < '0') || (c > '9')) { return false; }
                else
                {
                    x = x * 10;
                    decimalDiv = decimalDiv * 10;
                    x = x + (int)(c - '0');
                }
            }
            if (decimalDiv != 0) { x = x / decimalDiv; }
            content = x;
            return true;
        }
    }

    internal class Utilities
    {
        internal static bool TryParseUInt32(string s, out int result)
        {
            Contract.Requires(s != null);
            result = 0;
            if (s.Length > 0)
            {
                var r = 0;
                foreach (char c in s)
                {
                    if ((c < '0') || (c > '9')) { return false; }
                    var n = (int)(c - '0');
                    r = (r * 10) + n;
                }
                result = r;
                return true;
            }
            return false;
        }
    }
}
