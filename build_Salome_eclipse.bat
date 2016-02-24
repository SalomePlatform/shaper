@ECHO OFF

REM Find absolute path to ROOT_DIR
@SET PARENT_DIR=%~dp0..
@SET ROOT_DIR=
pushd %PARENT_DIR%
@SET ROOT_DIR=%CD%
popd

@SET SRC_DIR=%ROOT_DIR%\sources
@SET OCC_LIB_PREFIX=d
@SET SHAPER_ROOT_DIR=%ROOT_DIR%\install
@SET SalomeAppConfig=%ROOT_DIR%\install\share\salome\resources\shaper;%GUI_ROOT_DIR%\share\salome\resources\gui

call %SRC_DIR%\env_Salome.bat d
mkdir %ROOT_DIR%\build-eclipse
cd %ROOT_DIR%\build-eclipse

@SET CMAKE_ARGS=-G ^"Eclipse CDT4 - NMake Makefiles^"
@SET CMAKE_ARGS=%CMAKE_ARGS% -D_ECLIPSE_VERSION=4.3
@SET CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_BUILD_TYPE=Debug
@SET CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=OFF
@SET CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_INSTALL_PREFIX:PATH=%ROOT_DIR%\install
@SET CMAKE_ARGS=%CMAKE_ARGS% -DPYTHON_EXECUTABLE=%PYTHONHOME%\python_d.exe
@SET CMAKE_ARGS=%CMAKE_ARGS% %SRC_DIR%

cmake %CMAKE_ARGS%

@REM Should be defined at ..\local_env.bat
start %ECLIPSE_EXE%
