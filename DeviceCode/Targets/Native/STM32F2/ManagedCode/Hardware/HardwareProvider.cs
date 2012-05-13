using System;
using System.Threading;
using System.Runtime.CompilerServices;
using Microsoft.SPOT.Hardware;

namespace Microsoft.SPOT.Hardware.STM32F2
{
    internal class STM32F2HardwareProvider : HardwareProvider
    {
        static STM32F2HardwareProvider()
        {
            Microsoft.SPOT.Hardware.HardwareProvider.Register(new STM32F2HardwareProvider());
        }
    }
}
