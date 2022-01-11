@echo off
:: copy the USD and OmniClient libs to the $(OutputPath)
:: parameters:
:: 1. debug or release
:: 2. $(OutputPath)

set BUILD_CONFIG=%1
set OUTPUT_PATH=%2
set RC_OPTS=/NP /NJH /NJS /NS /NC /NFL /NDL

echo Copying deps\nv_usd\%BUILD_CONFIG%\lib contents to %OUTPUT_PATH%
robocopy /S deps\nv_usd\%BUILD_CONFIG%\lib %OUTPUT_PATH% %RC_OPTS%

echo Copying deps\omni_client_library\%BUILD_CONFIG% contents to %OUTPUT_PATH%
robocopy /S deps\omni_client_library\%BUILD_CONFIG% %OUTPUT_PATH% %RC_OPTS%

exit /B 0