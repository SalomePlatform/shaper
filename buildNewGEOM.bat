@SET ROOT_DIR=%~dp0..

@set SRC_DIR=%ROOT_DIR%\sources

call %SRC_DIR%\env.bat

mkdir %ROOT_DIR%\build
cd %ROOT_DIR%\build

cmake %SRC_DIR% -G "Visual Studio 10" -DCMAKE_INSTALL_PREFIX:PATH=%ROOT_DIR%\install
start "" %MSVC_EXE% NewGEOM.sln
