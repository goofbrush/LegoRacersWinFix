:: GETTING ADMIN PRIVILEGES
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if '%errorlevel%' NEQ '0' (
	echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"  
	echo UAC.ShellExecute "%~s0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"  
	"%temp%\getadmin.vbs"
	exit /B
)
::

cd %~dp0

Start "" "LEGORacers.exe" -novideo -alphatrans
Start /wait /b winfix.exe