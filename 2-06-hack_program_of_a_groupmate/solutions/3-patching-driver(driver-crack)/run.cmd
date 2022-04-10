@echo off

@rem https://github.com/lnslbrty/mingw-w64-ddk-template/blob/master/ddk-template-cplusplus-EASTL.bat

set SERVICE_NAME=mydriver
set DRIVER_PATH=C:\mydriver.sys

copy driver.sys %DRIVER_PATH%

sc create %SERVICE_NAME% binPath= %DRIVER_PATH%  type= kernel
echo ---------------------------------------
sc start %SERVICE_NAME%
echo ---------------------------------------
sc query %SERVICE_NAME%
echo [PRESS A KEY TO STOP THE DRIVER]
pause
sc stop %SERVICE_NAME%
sc delete %SERVICE_NAME%
echo Done.
timeout /t 3