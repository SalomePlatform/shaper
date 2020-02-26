@echo off

@REM Load local settings from file localenv.bat that should be located in the root directory
if "%ROOT_DIR%" == "" (
  set ROOT_DIR=%~dp0..
)

if exist "%ROOT_DIR%\localenv.bat" (
  echo Setting local environment from %ROOT_DIR%\localenv.bat
  call "%ROOT_DIR%\localenv.bat"
) else (
  echo Warning: file %ROOT_DIR%\localenv.bat does not exist; using default environment
)

if "%SALOME_ROOT_DIR%" == "" (
  set SALOME_ROOT_DIR=%ROOT_DIR%\SALOME
)

if "%PLANEGCS_ROOT_DIR%" == "" (
  SET PLANEGCS_ROOT_DIR=%SALOME_ROOT_DIR%\PRODUCTS%OCC_LIB_PREFIX%\planegcs-0.18
)
SET PATH=%PLANEGCS_ROOT_DIR%\lib;%PATH%

if "%EIGEN_ROOT_DIR%" == "" (
  SET EIGEN_ROOT_DIR=%SALOME_ROOT_DIR%\PRODUCTS%OCC_LIB_PREFIX%\eigen-3.2.7
)

if "%BOOST_ROOT_DIR%" == "" (
  if "SALOME_ROOT_DIR" == "" (
    SET BOOST_ROOT_DIR=%ROOT_DIR%\products\boost-1.67.0
  ) else (
    SET BOOST_ROOT_DIR=%SALOME_ROOT_DIR%\PRODUCTS%OCC_LIB_PREFIX%\boost-1.67.0
  )
)

cd /d %SALOME_ROOT_DIR%\WORK
call set_env.bat %1

if "%2" == run (
  call "%PDIR%\env_launch.bat"
) else (
  call "%PDIR%\env_compile.bat"
)

cd /d %ROOT_DIR%

@REM -------------------------
@REM Python
@SET PYTHON_LIB_DIR=%PYTHON_ROOT_DIR%\libs
@SET PYTHON_INC_DIR=%PYTHON_ROOT_DIR%\include
@REM -------------------------

@REM -------------------------
@REM CASCADE
@SET PATH=%CASROOT%;%CASROOT%\win64\vc14\bin%OCC_LIB_PREFIX%;%PATH%
@SET LIB=%CASROOT%\win64\vc14\lib%OCC_LIB_PREFIX%;%LIB%
@set CSF_MDTVFontDirectory=%CASROOT%\src\FontMFT
@set CSF_LANGUAGE=us
@set MMGT_CLEAR=1
@set CSF_EXCEPTION_PROMPT=1
@set CSF_SHMessage=%CASROOT%\src\SHMessage
@set CSF_MDTVTexturesDirectory=%CASROOT%\src\Textures
@set CSF_XSMessage=%CASROOT%\src\XSMessage
@set CSF_StandardDefaults=%CASROOT%\src\StdResource
@set CSF_PluginDefaults=%CASROOT%\src\StdResource
@set CSF_XCAFDefaults=%CASROOT%\src\StdResource
@set CSF_StandardLiteDefaults=%CASROOT%\src\StdResource
@set CSF_UnitsLexicon=%CASROOT%\src\UnitsAPI\Lexi_Expr.dat
@set CSF_UnitsDefinition=%CASROOT%\src\UnitsAPI\Units.dat
@set CSF_IGESDefaults=%CASROOT%\src\XSTEPResource
@set CSF_STEPDefaults=%CASROOT%\src\XSTEPResource
@set CSF_XmlOcafResource=%CASROOT%\src\XmlOcafResource
@REM -------------------------

@REM -------------------------
@SET PYTHONHOME=%PYTHON_ROOT_DIR%
@SET PYTHON_INCLUDE=%PYTHONHOME%\include
@REM -------------------------

@REM -------------------------
@REM Some OCCT headers include FREETYPE headers
@SET FREETYPE_ROOT_DIR=%PDIR%\freetype-2.9.0
@REM -------------------------

@SET PATH=%GEOM_ROOT_DIR%\lib\salome;%PATH%

@SET _NO_DEBUG_HEAP=1

@SET VC_VERSION_SHORT=15
for /f "tokens=1,2*" %%a in ('reg query "HKLM\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /v "%VC_VERSION_SHORT%.0" 2^>nul') do set "VSPATH=%%c"
IF NOT EXIST "%VSPATH%\VC\Auxiliary\Build\vcvarsall.bat" GOTO ERROR1

if exist "%VSPATH%\Common7\IDE\devenv.exe"  (
  SET msvc_exe=devenv
) else if exist "%VSPATH%\Common7\IDE\VCExpress.exe"  (
  SET msvc_exe=VCExpress
) else (
  GOTO ERROR1
)
call "%VSPATH%\VC\Auxiliary\Build\vcvarsall.bat" x64 > NUL

@SET SHAPER_ROOT_DIR=%ROOT_DIR%\install
@SET PATH=%SHAPER_ROOT_DIR%\lib\salome;%PATH%
REM @SET PYTHONPATH=%SHAPER_ROOT_DIR%\lib\python2.7\site-packages\salome\salome;%PYTHONPATH%
REM @SET PYTHONPATH=%SHAPER_ROOT_DIR%\bin\salome;%PYTHONPATH%

@SET LightAppConfig=%SHAPER_ROOT_DIR%\share\salome\resources\shaper;%GUI_ROOT_DIR%\share\salome\resources\gui

goto END

:ERROR1
ECHO "Visual Studio environment file is not found."

:END
