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

rem --------------- Configuration -------------- 

rem --------------- Products path --------------
if "%PRODUCTSDIR%" == "" (
  set PRODUCTSDIR=%ROOT_DIR%\PRODUCTS
)

rem --------------- cmake 2.8.7 --------------
if "%CMAKEDIR%" == "" (
  set CMAKEDIR=%PRODUCTSDIR%\cmake-2.8.12.1
)
set PATH=%CMAKEDIR%\bin;%PATH%

@REM -------------------------
@REM CASCADE
@SET CAS_ROOT_DIR=%PRODUCTSDIR%\OCCT-6.7.0
@SET CASROOT=%CAS_ROOT_DIR%
@REM -------------------------

@REM -------------------------
@REM CASCADE
@SET PATH=%CASROOT%;%CASROOT%\win32\bind;%PATH%
@set CSF_GraphicShr=%CASROOT%\win32\bind\TKOpenGl.dll
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
@REM FREEIMAGE (Used by OCCT)
@SET PATH=%PRODUCTSDIR%\freeimage-3.15.4\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM FTGL (Used by OCCT)
@SET PATH=%PRODUCTSDIR%\ftgl-2.1.3\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM FREETYPE (Used by OCCT)
@SET PATH=%PRODUCTSDIR%\freetype-2.4.11\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM GL2PS (Used by OCCT AND PARAVIEW)
@SET PATH=%PRODUCTSDIR%\gl2ps-1.3.8\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM TBB (Used by OCCT)
@SET PATH=%PRODUCTSDIR%\tbb\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM QT
if "%QTDIR%" == "" (
  @SET QTDIR=%PRODUCTSDIR%\Qt-5.2.0\5.2.0\msvc2010
)
@SET PATH=%QTDIR%\bin;%PATH%
@REM -------------------------

rem -------- Visual Studio --------------------
rem Detect Visual Studio (either commercial or Express edition)
if "%VS100COMNTOOLS%" == "" (
    echo Could not find MS Visual Studio: variable VS100COMNTOOLS is not defined!
    exit 1
) else if exist "%VS100COMNTOOLS%\..\IDE\devenv.exe" (
    set MSVC_EXE="%VS100COMNTOOLS%\..\IDE\devenv.exe"
) else if exist "%VS100COMNTOOLS%\..\IDE\VCExpress.exe" (
    set MSVC_EXE="%VS100COMNTOOLS%\..\IDE\VCExpress.exe"
) else (
    echo "Could not find MS Visual Studio in %VS100COMNTOOLS%\..\IDE"
    echo Check environment variable VS100COMNTOOLS!
    exit 1
)
call "%VS100COMNTOOLS%..\Tools\vsvars32.bat"
