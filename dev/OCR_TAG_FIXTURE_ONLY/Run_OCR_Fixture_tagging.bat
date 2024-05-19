@Rem Edited by Banna
@echo off
setlocal EnableDelayedExpansion
echo [93m============================="Created by Banna"===============================[0m
echo [93m=========================="OCR Fixture tagging V1"============================[0m

cd /D %~dp0
SET input_path=%~dp0input
SET output_path=%~dp0output
IF not exist %input_path% MD %input_path%
IF not exist %output_path% MD %output_path%
IF not exist DO_NOT_OPEN_OR_DELETE\model\input MD DO_NOT_OPEN_OR_DELETE\model\input
IF not exist DO_NOT_OPEN_OR_DELETE\model\output MD DO_NOT_OPEN_OR_DELETE\model\output


echo:
echo       USERNAME:  %USERDOMAIN%\%USERNAME%
set start_Time=%TIME%
echo       [93mStart Time: %start_Time%[0m

echo:
cd /D %~dp0

SET inCNT=0
SET outCNT=0
SET doneCNT=0

IF NOT EXIST "%input_path%\*.stl" (
    echo       [91m-- no STL files in "input".[0m
    timeout 5 
    EXIT
)

ECHO       [96m"Cleaning Temp"[0m
IF EXIST "DO_NOT_OPEN_OR_DELETE\model\input\*.stl" DEL /Q %~dp0\DO_NOT_OPEN_OR_DELETE\model\input\*.* >nul
IF EXIST "DO_NOT_OPEN_OR_DELETE\model\output\*.stl" DEL /Q %~dp0\DO_NOT_OPEN_OR_DELETE\model\output\*.* >nul
ECHO       [96m"Cleaning output"[0m
IF EXIST "output\*.stl" DEL /S /Q %~dp0output\*.* >nul
echo:

cd /D %~dp0
FOR /F "delims=" %%i IN ('dir /b /s %input_path%\*.stl') DO SET /a inCNT+=1
echo       [92m%inCNT%[0m  STL in "input" - [92mOriginal[0m

echo:
echo [93m================================"Adding Tag"==================================[0m
echo:

cd /D %~dp0DO_NOT_OPEN_OR_DELETE
FOR /F "delims=" %%i IN ('dir /b /s %input_path%\*.stl') DO (
    COPY /Y .\model\fixture\dummy.stl .\model\input\"%%~ni".stl  >nul
    CALL OCR_STL.exe "%%~ni" "-1.0"
    CALL PROCESS_OCR_STL.exe "%%~ni".stl

    DEL .\model\input\"%%~ni".stl >nul
    
    IF exist .\model\output\"%%~ni".stl (
        MOVE /Y .\model\output\"%%~ni".stl "%output_path%" >nul
        SET /a doneCNT+=1
        @REM echo       [92m++  !doneCNT!/%inCNT%[0m  %%~ni.stl exported [92msuccessfully[0m.
        echo       ++  !doneCNT!/%inCNT% - %%~ni.stl exported     OCR Tag added successfully.
    ) ELSE (
        @REM echo       [91m--  !doneCNT!/%inCNT%[0m  %%~ni.stl export [91mfailed[0m.
        echo       --  !doneCNT!/%inCNT% - %%~ni.stl export     OCR Tag adding failed.
    )
    echo:      
)

cd /D %~dp0

echo:
echo [93m================================="Finished"===================================[0m

FOR /F "delims=" %%i IN ('dir /b /s %output_path%\*.stl') DO SET /a outCNT+=1
IF %outCNT% GTR 0 (
    echo:
    echo       [92m%outCNT%/%inCNT%[0m  OCR Tag combined with fixture [92msuccessfully[0m.
    echo:
)
echo [93m=================================="REPORT"====================================[0m
echo:

echo       [92m%inCNT%[0m  STL in "input"  - Original
echo       [92m%outCNT%[0m  STL in "output" - Fixture with OCR Tag


SET stlCNTfailed=0
FOR /F "delims=" %%i IN ('dir /b /s %input_path%\*.stl') DO (
    IF NOT EXIST %output_path%\"%%~ni".stl (
	    SET /a stlCNTfailed+=1
        echo:
        echo       [91m--[0m %%~ni.stl export with OCR Tag and fixture [91mfailed.[0m
    )
)
IF %stlCNTfailed% GTR 0 (
    echo:
    echo       [91m%stlCNTfailed%/%inCNT%[0m  stl create with OCR Tag and fixture [91mFailed[0m.
)

echo:
set end_time=%time%
set /a hours=%end_time:~0,2%-%start_time:~0,2%
set /a minutes=%end_time:~3,2%-%start_time:~3,2%
set /a seconds=%end_time:~6,2%-%start_time:~6,2%
set /a total_seconds=%hours%*3600 + %minutes%*60 + %seconds%
set /a minutes=%total_seconds% / 60
set /a seconds=%total_seconds% %% 60
echo:
echo       USERNAME:  %USERDOMAIN%\%USERNAME%
echo:
echo       [93mElapsed:  %minutes% minutes %seconds% seconds[0m
echo:
echo       Start  :  %start_Time%
echo       Finish :  %time%
echo:
pause