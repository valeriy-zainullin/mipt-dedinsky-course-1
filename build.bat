set CURDIR=%cd%
for %%f in (%CURDIR%\*.c) do (
	cmd /c "cd C:\cygwin64\bin && x86_64-w64-mingw32-gcc -Wall -pedantic -Wextra -Wshadow -Werror -O2 -std=c99 -static %%f -o %%~df%%~pf%%~nf.exe"
)