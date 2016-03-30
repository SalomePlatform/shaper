@ECHO OFF

REM Find absolute path to ROOT_DIR 
@SET PARENT_DIR=%~dp0..
@SET ROOT_DIR=
pushd %PARENT_DIR%
@SET ROOT_DIR=%CD%
popd

@SET SRC_DIR=%ROOT_DIR%\sources
@SET OCC_LIB_PREFIX=d

call %SRC_DIR%\env_Salome.bat d
mkdir %ROOT_DIR%\build
cd %ROOT_DIR%\build

cmake %SRC_DIR% -G "Visual Studio 10 Win64" -DPYTHON_EXECUTABLE=%PYTHONHOME%\python_d.exe -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=%ROOT_DIR%\install

start "" %MSVC_EXE% SHAPER.sln
