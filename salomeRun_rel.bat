@ECHO OFF

REM Find absolute path to ROOT_DIR
@SET PARENT_DIR=%~dp0..
@SET ROOT_DIR=
pushd %PARENT_DIR%
@SET ROOT_DIR=%CD%
popd

call env_Salome.bat

@SET SHAPER_ROOT_DIR=%ROOT_DIR%\install
@SET SalomeAppConfig=%SHAPER_ROOT_DIR%\share\salome\resources\shaper;%GUI_ROOT_DIR%\share\salome\resources\gui

start %PYTHONBIN% "%KERNEL_ROOT_DIR%\bin\salome\envSalome.py" "%PYTHONBIN%" "%KERNEL_ROOT_DIR%\bin\salome\runSalome.py" --modules=SHAPER,GEOM,SMESH %*
