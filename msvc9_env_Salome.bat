@echo off

IF NOT EXIST "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" GOTO ERROR1

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

call "%SALOME_ROOT_DIR%\WORK\set_env.bat"
call "%PDIR%\env_compile.bat"

@REM -------------------------
@REM Python
@SET PYTHON_LIB_DIR=%PYTHON_ROOT_DIR%\libs
@SET PYTHON_INC_DIR=%PYTHON_ROOT_DIR%\include
@REM -------------------------

@REM -------------------------
@REM CASCADE
@SET PATH=%CASROOT%;%CASROOT%\win32\bin%OCC_LIB_PREFIX%;%PATH%
@SET LIB=%CASROOT%\win32\lib%OCC_LIB_PREFIX%;%LIB%
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
@REM Create launch path
@SET PATH=%LIBXML2_ROOT_DIR%\bin;%PATH%
@SET PATH=%PDIR%\tbb\bin;%PATH%
@SET PATH=%PDIR%\freeimage-3.15.4\bin;%PATH%
@SET PATH=%PDIR%\ftgl-2.1.3\bin;%PATH%
@SET PATH=%PDIR%\freetype-2.4.11\bin;%PATH%
@SET PATH=%PDIR%\gl2ps-1.3.8\bin;%PATH%
@SET PATH=%PDIR%\qwt-5.2.1\lib;%PATH%
@REM -------------------------

IF "%ARCH%" == "Win64" (
  call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" x64
) ELSE (
  IF "%ARCH%" == "Win32" (
    call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" x86
  ) ELSE (
    echo Wrong architecture is used. Win32 or Win64 architecture is allowed only.
    echo Refer to the set_env.bat script.
  )
)

@SET NEW_GEOM_CONFIG_FILE=%ROOT_DIR%\install\plugins
@SET PATH=%ROOT_DIR%\install\bin;%ROOT_DIR%\install\plugins;%PATH%

@SET LightAppConfig=%ROOT_DIR%\install\share\salome\resources\newgeom;%GUI_ROOT_DIR%\share\salome\resources\gui
