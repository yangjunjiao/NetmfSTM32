//Copyright 2012 Cuno Pfister
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

//Developed for the book
//  "Getting Started with the Internet of Things", by Cuno Pfister.
//  Copyright 2012 Cuno Pfister, Inc., 978-1-4493-9357-1.
//
//Version 1.0 (for NETMF 4.2)

using System;
using System.IO;
using System.Net;
using System.Text;
using Microsoft.SPOT;

namespace Gsiot.CosmClient
{
    public static class CosmClient
    {
        const string baseUri = "http://api.cosm.com/v2/feeds/";

        public static void Send(string apiKey, string feedId,
            string sample)
        {
            Debug.Print("time: " + DateTime.Now);
            Debug.Print("memory available: " + Debug.GC(true));
            try
            {
                using (var request = CreateRequest(apiKey, feedId, sample))
                {
                    request.Timeout = 5000;     // 5 seconds
                    // send request and receive response
                    using (var response =
                        (HttpWebResponse)request.GetResponse())
                    {
                        HandleResponse(response);
                    }
                }
            }
            catch (Exception e)
            {
                Debug.Print(e.ToString());
            }
        }

        static HttpWebRequest CreateRequest(string apiKey, string feedId,
            string sample)
        {
            byte[] buffer = Encoding.UTF8.GetBytes(sample);

            var request = (HttpWebRequest)WebRequest.Create
                (baseUri + feedId + ".csv");

            // request line
            request.Method = "PUT";

            // request headers
            request.ContentLength = buffer.Length;
            request.ContentType = "text/csv";
            request.Headers.Add("X-ApiKey", apiKey);

            // request body
            using (Stream stream = request.GetRequestStream())
            {
                stream.Write(buffer, 0, buffer.Length);
            }

            return request;
        }

        public static void HandleResponse(HttpWebResponse response)
        {
            Debug.Print("Status code: " + response.StatusCode);
        }
    }
}
