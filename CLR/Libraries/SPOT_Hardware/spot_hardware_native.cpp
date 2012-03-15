////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SPOT_Hardware.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Cpu::get_SystemClock___STATIC__U4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Cpu::get_SlowClock___STATIC__U4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Cpu::get_GlitchFilterTime___STATIC__mscorlibSystemTimeSpan,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Cpu::set_GlitchFilterTime___STATIC__VOID__mscorlibSystemTimeSpan,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Battery::ReadVoltage___STATIC__I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Battery::ReadTemperature___STATIC__I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Battery::OnCharger___STATIC__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Battery::IsFullyCharged___STATIC__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Battery::StateOfCharge___STATIC__I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Battery::WaitForEvent___STATIC__BOOLEAN__I4,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Battery__ChargerModel::_ctor___VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_I2CDevice::Execute___I4__SZARRAY_MicrosoftSPOTHardwareI2CDeviceI2CTransaction__I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_I2CDevice::Initialize___VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_SPI::InternalWriteRead___VOID__SZARRAY_U2__I4__I4__SZARRAY_U2__I4__I4__I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_SPI::InternalWriteRead___VOID__SZARRAY_U1__I4__I4__SZARRAY_U1__I4__I4__I4,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_NativeEventDispatcher::EnableInterrupt___VOID,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_NativeEventDispatcher::DisableInterrupt___VOID,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_NativeEventDispatcher::Dispose___VOID__BOOLEAN,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_NativeEventDispatcher::_ctor___VOID__STRING__U8,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_EventSink::EventConfig___VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Port::Dispose___VOID__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Port::_ctor___VOID__MicrosoftSPOTHardwareCpuPin__BOOLEAN__MicrosoftSPOTHardwarePortResistorMode__MicrosoftSPOTHardwarePortInterruptMode,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Port::_ctor___VOID__MicrosoftSPOTHardwareCpuPin__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Port::_ctor___VOID__MicrosoftSPOTHardwareCpuPin__BOOLEAN__BOOLEAN__MicrosoftSPOTHardwarePortResistorMode,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Port::Read___BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Port::get_Id___MicrosoftSPOTHardwareCpuPin,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Port::ReservePin___STATIC__BOOLEAN__MicrosoftSPOTHardwareCpuPin__BOOLEAN,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_LargeBufferMarshaller::MarshalBuffer___VOID__MicrosoftSPOTHardwareLargeBuffer,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_LargeBufferMarshaller::UnMarshalBuffer___VOID__BYREF_MicrosoftSPOTHardwareLargeBuffer,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_AnalogInput::ReadRaw___I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_AnalogInput::Initialize___STATIC__VOID__MicrosoftSPOTHardwareCpuAnalogChannel__I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetSerialPins___VOID__I4__BYREF_MicrosoftSPOTHardwareCpuPin__BYREF_MicrosoftSPOTHardwareCpuPin__BYREF_MicrosoftSPOTHardwareCpuPin__BYREF_MicrosoftSPOTHardwareCpuPin,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetSerialPortsCount___I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeSupportsNonStandardBaudRate___BOOLEAN__I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetBaudRateBoundary___VOID__I4__BYREF_U4__BYREF_U4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeIsSupportedBaudRate___BOOLEAN__I4__BYREF_U4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetSpiPins___VOID__MicrosoftSPOTHardwareSPISPImodule__BYREF_MicrosoftSPOTHardwareCpuPin__BYREF_MicrosoftSPOTHardwareCpuPin__BYREF_MicrosoftSPOTHardwareCpuPin,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetSpiPortsCount___I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetI2CPins___VOID__BYREF_MicrosoftSPOTHardwareCpuPin__BYREF_MicrosoftSPOTHardwareCpuPin,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetPinsCount___I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetPinsMap___VOID__SZARRAY_MicrosoftSPOTHardwareCpuPinUsage,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetPinUsage___MicrosoftSPOTHardwareCpuPinUsage__MicrosoftSPOTHardwareCpuPin,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetSupportedResistorModes___MicrosoftSPOTHardwareCpuPinValidResistorMode__MicrosoftSPOTHardwareCpuPin,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetSupportedInterruptModes___MicrosoftSPOTHardwareCpuPinValidInterruptMode__MicrosoftSPOTHardwareCpuPin,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetButtonPins___MicrosoftSPOTHardwareCpuPin__MicrosoftSPOTHardwareButton,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetLCDMetrics___VOID__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetPWMChannelsCount___I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetPWMPinForChannel___MicrosoftSPOTHardwareCpuPin__MicrosoftSPOTHardwareCpuPWMChannel,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetAnalogChannelsCount___I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetAnalogPinForChannel___MicrosoftSPOTHardwareCpuPin__MicrosoftSPOTHardwareCpuAnalogChannel,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_HardwareProvider::NativeGetAvailablePrecisionInBitsForChannel___SZARRAY_I4__MicrosoftSPOTHardwareCpuAnalogChannel,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InputPort::get_Resistor___MicrosoftSPOTHardwarePortResistorMode,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InputPort::set_Resistor___VOID__MicrosoftSPOTHardwarePortResistorMode,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InputPort::get_GlitchFilter___BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InputPort::set_GlitchFilter___VOID__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InterruptPort::EnableInterrupt___VOID,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InterruptPort::DisableInterrupt___VOID,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InterruptPort::ClearInterrupt___VOID,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InterruptPort::get_Interrupt___MicrosoftSPOTHardwarePortInterruptMode,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_InterruptPort::set_Interrupt___VOID__MicrosoftSPOTHardwarePortInterruptMode,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_LargeBuffer::InternalCreateBuffer___VOID__I4,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_LargeBuffer::InternalDestroyBuffer___VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_OutputPort::Write___VOID__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_OutputPort::get_InitialState___BOOLEAN,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::Reboot___STATIC__VOID__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::WaitForIdleCPU___STATIC__BOOLEAN__I4__I4,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::get_MaximumTimeToActive___STATIC__mscorlibSystemTimeSpan,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::set_MaximumTimeToActive___STATIC__VOID__mscorlibSystemTimeSpan,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::get_WakeupEvents___STATIC__MicrosoftSPOTHardwareHardwareEvent,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::set_WakeupEvents___STATIC__VOID__MicrosoftSPOTHardwareHardwareEvent,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::get_Uptime___STATIC__mscorlibSystemTimeSpan,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::InternalSleep___STATIC__VOID__MicrosoftSPOTHardwareSleepLevel__MicrosoftSPOTHardwareHardwareEvent,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_PowerState::InternalChangePowerLevel___STATIC__VOID__MicrosoftSPOTHardwarePowerLevel,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_TristatePort::get_Active___BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_TristatePort::set_Active___VOID__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_TristatePort::get_Resistor___MicrosoftSPOTHardwarePortResistorMode,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_TristatePort::set_Resistor___VOID__MicrosoftSPOTHardwarePortResistorMode,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_TristatePort::get_GlitchFilter___BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::get_Enabled___STATIC__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::set_Enabled___STATIC__VOID__BOOLEAN,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::get_Timeout___STATIC__mscorlibSystemTimeSpan,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::set_Timeout___STATIC__VOID__mscorlibSystemTimeSpan,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::get_Behavior___STATIC__MicrosoftSPOTHardwareWatchdogBehavior,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::set_Behavior___STATIC__VOID__MicrosoftSPOTHardwareWatchdogBehavior,
    NULL,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::get_Log___STATIC__MicrosoftSPOTNativeMicrosoftSPOTILog,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::set_Log___STATIC__VOID__MicrosoftSPOTNativeMicrosoftSPOTILog,
    Library_spot_hardware_native_Microsoft_SPOT_Hardware_Watchdog::GetLastOcurrenceDetails___STATIC__BOOLEAN__BYREF_mscorlibSystemDateTime__BYREF_mscorlibSystemTimeSpan__BYREF_mscorlibSystemReflectionMethodInfo,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Microsoft_SPOT_Hardware =
{
    "Microsoft.SPOT.Hardware", 
    0x7765AC69,
    method_lookup
};

