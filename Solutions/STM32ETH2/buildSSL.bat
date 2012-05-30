


cd D:\MicroFrameworkPK_v4_2
setenv_MDK4.12.cmd C:\Keil\ARM
cd D:\MicroFrameworkPK_v4_2\Solutions\STM32ETH2

msbuild dotNetMF.proj /t:build /p:flavor=debug;TCP_IP_STACK=LWIP;memory=FLASH;platform=STM32ETH2;SSL=OPEN_SSL