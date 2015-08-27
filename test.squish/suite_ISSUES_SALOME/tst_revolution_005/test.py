def main():
    #[project] NewGEOM
    #[Scenario] Revolution_005
    #[Topic] 'RevolutionFuse by planes and offsets' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_revolution_005.hdf'
    open(DATA_PATH + "/for_revolution_005.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Not loaded Part\\_1", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Fit all
    fit_all()
    
    #[step] Execute RevolutionFuse operation
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "RevolutionFuse"))
    
    #[step] Select sketch face
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 287, 236, 0, Qt.LeftButton)
    
    # [step] Create 2 circles
    circle_create_in_view((523, 154), (539, 168))
    circle_create_in_view((528, 340), (537, 351))
    
    #[step] Approve sketching
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
    #[step] Select axis for revolution  
    mouseClick(waitForObject(":Revolution_QLineEdit_2"), 128, 12, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 652, 257, 0, Qt.LeftButton)
    
    #[step] Rotate the model
    clickButton(waitForObject(":Revolution_QToolButton_3"))
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 456, 470, -61, -138, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 497, 341, -56, -2, 67108868, Qt.MidButton)
    
    #[step] Fit all
    fit_all()

    # [step] Select face 'To' 
    mouseClick(waitForObject(":To_QLineEdit_3"), 30, 8, 0, Qt.LeftButton)
    snooze(1)
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonPress, 694, 187, Qt.LeftButton, 1, 0)
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonRelease, 694, 187, Qt.LeftButton, 0, 0)

    #[step] Fit all
    fit_all()
    
    #[check] Check that preview is updated
    test.vp("VP1")
    
    #[step] Define offset for face 'To' 
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_3"), 34, 9, 0, Qt.LeftButton)
    type(waitForObject(":To.to_offset_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":To.to_offset_ModuleBase_ParamSpinBox_3"), 30)

    #[step] Fit all
    fit_all()
    
    #[check] Check that preview is updated
    test.vp("VP2")
    
    #[step] Select face 'From'
    mouseClick(waitForObject(":From_QLineEdit_3"), 28, 11, 0, Qt.LeftButton)

    #[step] Click '-OX' button
    clickButton(waitForObject(":SALOME*.-OX_QToolButton"))
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonPress, 441, 217, Qt.LeftButton, 1, 0)
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonRelease, 441, 217, Qt.LeftButton, 0, 0)
    
    #[step] Rotate the model
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 576, 380, -99, -156, 67108866, Qt.RightButton)
    
    #[step] Fit all
    fit_all()
    
    #[check] Check that preview is updated
    test.vp("VP3")
    
    #[step] Define offset for face 'From'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_4"), 38, 4, 0, Qt.LeftButton)
    type(waitForObject(":From.from_offset_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":From.from_offset_ModuleBase_ParamSpinBox_3"), 45)

    #[check] Check that preview is updated
    test.vp("VP4")
    
    #[step] Approve sketching    
    clickButton(waitForObject(":RevolutionFuse.property_panel_ok_QToolButton"))
    
    #[step] Rotate the model
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 103, 388, 309, -313, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 512, 266, -75, 52, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 555, 60, 33, 113, 67108868, Qt.MidButton)
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))

    #[step] Fit all
    fit_all()

    # [step] Close application without saving
    close_application()
