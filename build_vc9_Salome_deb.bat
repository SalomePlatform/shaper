@ECHO OFF

REM Find absolute path to ROOT_DIR 
@SET PARENT_DIR=%~dp0..
@SET ROOT_DIR=
pushd %PARENT_DIR%
@SET ROOT_DIR=%CD%
popd

@SET SRC_DIR=%ROOT_DIR%\sources
@SET OCC_LIB_PREFIX=d

call %SRC_DIR%\msvc9_env_Salome.bat
mkdir %ROOT_DIR%\build
cd %ROOT_DIR%\build

cmake %SRC_DIR% -G "Visual Studio 9 2008" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=%ROOT_DIR%\install

start "" %MSVC_EXE% NewGEOM.sln
