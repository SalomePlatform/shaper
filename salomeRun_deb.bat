@ECHO OFF

@SET OCC_LIB_PREFIX=d
call env_Salome.bat d

@SET SHAPER_ROOT_DIR=%ROOT_DIR%\install
@SET SalomeAppConfig=%SHAPER_ROOT_DIR%\share\salome\resources\shaper;%GUI_ROOT_DIR%\share\salome\resources\gui

start %PYTHONBIN% "%KERNEL_ROOT_DIR%\bin\salome\envSalome.py" "%PYTHONBIN%" "%KERNEL_ROOT_DIR%\bin\salome\runSalome.py" %*
