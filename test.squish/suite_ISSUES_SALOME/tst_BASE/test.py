def main():
    startApplication("salome_run.sh")
    
    clickButton(waitForObject(":SALOME 7.6.0.NewGeom_QToolButton"))
    clickButton(waitForObject(":Activate module.New_QPushButton"))
    clickButton(waitForObject(":SALOME 7.6.0 - [Study1].Sketch_QToolButton"))
    mouseClick(waitForObject(":SALOME 7.6.0 - [Study1].3D View Operations_OCCViewer_ViewPort3d"), 437, 318, 0, Qt.LeftButton)
    clickButton(waitForObject(":SALOME 7.6.0 - [Study1].Circle_QToolButton"))
    mouseClick(waitForObject(":SALOME 7.6.0 - [Study1].3D View Operations_OCCViewer_ViewPort3d"), 384, 278, 0, Qt.LeftButton)
    sendEvent("QMouseEvent", waitForObject(":SALOME 7.6.0 - [Study1].3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonPress, 556, 381, Qt.LeftButton, 1, 0)
    sendEvent("QMouseEvent", waitForObject(":SALOME 7.6.0 - [Study1].3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonRelease, 556, 381, Qt.LeftButton, 0, 0)
    clickButton(waitForObject(":SALOME 7.6.0 - [Study1].Circle_QToolButton"))
    clickButton(waitForObject(":SALOME 7.6.0 - [Study1]_QToolButton"))
  
    sendEvent("QCloseEvent", waitForObject(":SALOME 7.6.0 - [Study1]_STD_TabDesktop"))
    clickButton(waitForObject(":Exit.Shutdown servers_QCheckBox"))
    sendEvent("QMoveEvent", waitForObject(":Exit_SalomeApp_ExitDlg"), 502, 326, 1265, 44)
    clickButton(waitForObject(":Exit.Ok_QPushButton"))
    clickButton(waitForObject(":Close active study.Close w/o saving_QPushButton"))
