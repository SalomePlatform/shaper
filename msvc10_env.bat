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
if "%PDIR%" == "" (
  set PDIR=%ROOT_DIR%\products
)

rem --------------- cmake 2.8.7 --------------
if "%CMAKEDIR%" == "" (
  set CMAKEDIR=%PDIR%\cmake-2.8.10.2
)
set PATH=%CMAKEDIR%\bin;%PATH%

@REM -------------------------
@REM CASCADE
@SET CAS_ROOT_DIR=%PDIR%\OCCT-6.8.0
@SET CASROOT=%CAS_ROOT_DIR%
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
@REM FREEIMAGE (Used by OCCT)
@SET PATH=%PDIR%\freeimage-3.15.4\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM FTGL (Used by OCCT)
@SET PATH=%PDIR%\ftgl-2.1.3\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM FREETYPE (Used by OCCT)
@SET PATH=%PDIR%\freetype-2.4.11\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM GL2PS (Used by OCCT AND PARAVIEW)
@SET PATH=%PDIR%\gl2ps-1.3.8\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM TBB (Used by OCCT)
@SET PATH=%PDIR%\tbb\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM QT
@SET QTDIR=%PDIR%\qt-4.8.4
@ECHO -- Creating qt.conf... in %QTDIR%
@ECHO [Paths] > %QTDIR%/bin/qt.conf
@ECHO Prefix = %QTDIR:\=/% >> %QTDIR%/bin/qt.conf
@SET PATH=%QTDIR%\bin;%PATH%

@REM -------------------------

@REM -------------------------
@REM PYTHON
@SET PYTHONHOME=%PDIR%\python-2.7.3
@SET PYTHON_INCLUDE=%PYTHONHOME%\include
@SET PATH=%PYTHONHOME%;%PYTHONHOME%\libs;%PATH%
@SET PYTHONPATH=%PYTHONHOME%\Lib;%PYTHONPATH%
@SET PYTHONPATH=%PYTHONHOME%\DLLs;%PYTHONPATH%
@SET PYTHON_VERSION=2.7
@REM -------------------------

@REM -------------------------
@REM ZLIB (for LIBXML2)
@SET PATH=%PDIR%\zlib-1.2.5\dll;%PATH%
@REM -------------------------

@REM -------------------------
@REM LIBXML2
@SET LIBXML2_ROOT_DIR=%PDIR%\libxml2-2.9.0
@SET PATH=%LIBXML2_ROOT_DIR%\lib;%PATH%
@SET PATH=%LIBXML2_ROOT_DIR%\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM SWIG
@SET PATH=%PDIR%\swig-2.0.9\bin;%PATH%
@REM -------------------------

@REM -------------------------
@SET SOLVESPACE_ROOT_DIR=%PDIR%\solvespace-2.0
@SET PATH=%SOLVESPACE_ROOT_DIR%\lib;%PATH%
@REM -------------------------

@SET NEW_GEOM_CONFIG_FILE=%ROOT_DIR%\install\plugins
@SET PATH=%ROOT_DIR%\install\plugins;%ROOT_DIR%\install\bin;%PATH%
@SET PYTHONPATH=%ROOT_DIR%\install\swig;%ROOT_DIR%\install\plugins;%PYTHONPATH%

@REM -------------------------
@REM PTHREAD
@SET PATH=%PDIR%\pthreads-2.9.1\lib;%PATH%
@REM -------------------------

@REM -------------------------
@REM DOXYGEN 
@SET DOXYGEN_ROOT_DIR=%PDIR%\doxygen-1.8.3.1
@SET PATH=%DOXYGEN_ROOT_DIR%\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM GRAPHVIZ 
@SET GRAPHVIZ_ROOT_DIR=%PDIR%\graphviz-2.38.0
@SET GRAPHVIZHOME=%GRAPHVIZ_ROOT_DIR%
@SET PATH=%GRAPHVIZ_ROOT_DIR%\bin;%PATH%
@REM -------------------------

@REM -------------------------
@REM SUIT
@SET SUIT_DIR=%PDIR%\suit
@SET PATH=%SUIT_DIR%\bin;%PATH%
@REM -------------------------


@REM -------------------------
if "%GUI_ROOT_DIR%" == "" (
  @SET GUI_ROOT_DIR=%ROOT_DIR%\SALOME\GUI\DEBUG\GUI_INSTALL
)
@REM -------------------------


@SET NewGeomResources=%ROOT_DIR%\install\resources

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
