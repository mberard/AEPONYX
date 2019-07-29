@echo OFF
REM ***********************************************************
REM * Start training                                          *
REM * A bat file that automatically starts training material  *
REM ***********************************************************
REM *                                                         *
REM * Please set the following three variables below:         *
REM * IPKISS_VERSION:                                         *
REM *    The default (recommended) ipkiss version             *
REM *    for this training                                    *
REM * TRAINING_PATH:                                          *
REM *    The path to the training material.                   *
REM *    By default it points to the location where           *
REM *    this file is present (%~dp0)                         *
REM * ADDITIONAL_PYTHONPATH (optional, can be left empty):    *
REM *    Any additional paths to the PYTHONPATH               *
REM *    This is useful in case you want to distribute        *
REM *    python packages alongside this training              *
REM *    (such as a PDK, ...)                                 *
REM *    Use semicolumns to separate. Paths are relative      *
REM *    to the location of this bat file.                    *
REM *                                                         *
REM ***********************************************************

set IPKISS_VERSION=3.1.3
set TRAINING_PATH=%~dp0
set ADDITIONAL_PYTHONPATH=

REM ***********************************************************
REM * Do not modify the code below                            *
REM * to ensure proper operation                              *
REM ***********************************************************


set VERSION=%IPKISS_VERSION%

:parse
IF "%~1"=="-v" GOTO changeversion
GOTO getinstallpath
:parse2
IF "%~1"=="-h" GOTO usage
GOTO runtraining

:changeversion
set VERSION=%~2
SHIFT
SHIFT
echo "Changed version to %VERSION%"
GOTO :parse

:getinstallpath

set INSTALL_PATH=Not found
echo Looking for IPKISS %VERSION% ...

IF %VERSION%==3.0.1 (
    SET NOTEBOOK_COMMAND=ipython
    FOR /F "tokens=3 delims= " %%A IN ('REG QUERY "HKCU\Software\Python\PythonCore\2.7 For IPKISS\InstallPath" /ve') DO SET INSTALL_PATH=%%A
) ELSE (
    SET NOTEBOOK_COMMAND=jupyter
    IF %VERSION%==3.1.1 (
        REM Note: the registry key for ipkiss 3.1.1 was stored in IPKISS\3.1.0\InstallPath! Hence this exception.
        FOR /F "tokens=3 delims= " %%A IN ('REG QUERY "HKCU\Software\Python\PythonCore\2.7 For IPKISS\3.1.0\InstallPath" /ve') DO SET INSTALL_PATH=%%A
    ) ELSE (
        reg query "HKCU\Software\Python\PythonCore\2.7 For IPKISS\%VERSION%\InstallPath"
       
        FOR /F "tokens=3 delims= " %%A IN ('REG QUERY "HKCU\Software\Python\PythonCore\2.7 For IPKISS\%VERSION%\InstallPath" /ve') DO SET INSTALL_PATH=%%A
    )
)

if "%INSTALL_PATH%"=="Not found" (
	IF %VERSION% NEQ 3.1.0 GOTO find_next
    echo No IPKISS installation was found. Have you installed the luceda software?
    echo Note: this training is designed to run on version %IPKISS_VERSION%. This can be overridden using the '-v' option.
    echo The following versions have been looked for: 3.1.2 3.1.1 3.1.0
    GOTO usage
)

GOTO :parse2

:find_next


if %INSTALL_PATH%=="Not found" (
    IF %VERSION%==3.1.2 (
        set VERSION=3.1.1
        goto :getinstallpath
        )
    IF %VERSION%==3.1.1 (
        set VERSION=3.1.0
        goto :getinstallpath
    )
)

:runtraining

IF DEFINED ADDITIONAL_PYTHONPATH (
    SET NEW_PYTHONPATH=%ADDITIONAL_PYTHONPATH%;%PYTHONPATH%
) ELSE (
    SET NEW_PYTHONPATH=%PYTHONPATH%
)

echo %INSTALL_PATH%
set SCRIPTPATH=%INSTALL_PATH%\..\..\Scripts
set CONDAENV=ipkiss3
set PYTHONPATH=%NEW_PYTHONPATH%
echo "PYTHONPATH = %PYTHONPATH%"
set PATH=%SCRIPTPATH%;%PATH% && activate %CONDAENV% && cd %TRAINING_PATH% && %NOTEBOOK_COMMAND% notebook %TRAINING_PATH%/Welcome.ipynb

GOTO :eof

:usage
echo(
echo Usage: start_training.bat [-v 3.1.0] [-h]
echo Optional arguments
echo   -v [version]: force to use a certain version
echo   -h          : display this help
echo(
echo Please verify the following four items:
echo 1. Path to the ipkiss software for this training (version: %VERSION%): %INSTALL_PATH%.
echo 2. Path to the training material: %TRAINING_PATH%.
echo 3. Command to start the notebook: %NOTEBOOK_COMMAND% notebook Welcome.ipynb
echo 4. PYTHONPATH to start training: %NEW_PYTHONPATH%

IF INSTALL_PATH==Not found (
    echo This script will pause so this message can be shown before closing down.
    pause
)