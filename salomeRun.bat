@ECHO OFF

call env_Salome.bat

@SET NewGEOM_ROOT_DIR=%ROOT_DIR%\install
@SET SalomeAppConfig=%ROOT_DIR%\install\share\salome\resources\newgeom;%GUI_ROOT_DIR%\share\salome\resources\gui

start %PYTHONBIN% "%KERNEL_ROOT_DIR%\bin\salome\envSalome.py" "%PYTHONBIN%" "%KERNEL_ROOT_DIR%\bin\salome\runSalome.py" %*
