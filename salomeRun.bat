@ECHO OFF

REM Find absolute path to ROOT_DIR 
@SET PARENT_DIR=%~dp0..
@SET ROOT_DIR=
pushd %PARENT_DIR%
@SET ROOT_DIR=%CD%
popd

@SET SRC_DIR=%ROOT_DIR%\sources
@SET OCC_LIB_PREFIX=d

@SET SALOME_ROOT_DIR=%ROOT_DIR%\SALOME-7.3.0-WIN32
@SET SOLVESPACE_ROOT_DIR=%ROOT_DIR%\products\solvespace-2.0
@SET NEWGEOM_ROOT_DIR=%ROOT_DIR%\install
@SET PATH=%SALOME_ROOT_DIR%;%PATH%


call %SALOME_ROOT_DIR%\env_launch.bat
rem Unfortunately in "env_launch.bat" only KERNEL is added as a module, so add GUI manually
call %SALOME_ROOT_DIR%\set_one_module_env.bat GUI

@SET PATH=%SOLVESPACE_ROOT_DIR%\lib;%PATH%
@SET NEW_GEOM_CONFIG_FILE=%ROOT_DIR%\install\plugins
@SET PATH=%PATH%;%NEWGEOM_ROOT_DIR%\bin;%NEWGEOM_ROOT_DIR%\plugins

@SET LightAppConfig=%ROOT_DIR%\install\share\salome\resources\newgeom;%SALOME_ROOT_DIR%\MODULES\GUI\share\salome\resources\gui


SuitApp.exe LightApp -style salome --modules=NewGeom --uselicense --noexcepthandling
