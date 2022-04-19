bcdedit /debug on
bcdedit /dbgsettings serial DEBUGPORT:1 BAUDRATE:115200
bcdedit /set testsigning on
bcdedit /dbgsettings
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Debug Print Filter" /v DEFAULT /t REG_DWORD /d 0xffffffff /f
pause