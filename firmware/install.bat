@echo off
cd /d %~dp0

if "%PROCESSOR_ARCHITECTURE%" EQU "x86" (
	set ROOT_DIR="%ProgramFiles%\Arduino"
) else (
	set ROOT_DIR="%ProgramFiles(x86)%\Arduino"
)

rem hardware
set DEST="%ROOT_DIR%\hardware\RokkoOroshiCanServoBoard"
if not exist "%DEST%" (
	mklink /D "%DEST%" "%cd%\RokkoOroshiCanServoBoard"
) else (
	echo "%DEST% is already exist!"
)

rem examples

set DEST="%ROOT_DIR%\examples\RokkoOroshiCanServoBoard"
if not exist "%DEST%" (
	mklink /D "%DEST%" "%cd%\examples"
) else (
	echo "%DEST% is already exist!"
)

rem externals

set DEST="%ROOT_DIR%\libraries\CAN_BUS_Shield"
if not exist "%DEST%" (
	mklink /D "%DEST%" "%cd%\external_library\CAN_BUS_Shield"
) else (
	echo "%DEST% is already exist!"
)

set DEST="%ROOT_DIR%\libraries\MsTimer2"
if not exist "%DEST%" (
	mklink /D "%DEST%" "%cd%\external_library\MsTimer2"
) else (
	echo "%DEST% is already exist!"
)

set DEST="%ROOT_DIR%\libraries\Arduino-SerialCommand"
if not exist "%DEST%" (
	mklink /D "%DEST%" "%cd%\external_library\Arduino-SerialCommand"
) else (
	echo "%DEST% is already exist!"
)


pause
