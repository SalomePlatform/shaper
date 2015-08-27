def main():
    #[project] NewGEOM
    #[Scenario] Revolution_004
    #[Topic] 'RevolutionFuse by angles' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_revolution_004.hdf'
    open(DATA_PATH + "/for_revolution_004.hdf")
    
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
    circle_create_in_view((650, 364), (671, 392))
    circle_create_in_view((641, 114), (664, 141))
    
    #[step] Approve sketching
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
    #[step] Select axis for revolution
    mouseClick(waitForObject(":Revolution_QLineEdit_2"), 44, 14, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 750, 203, 0, Qt.LeftButton)
    
    #[step] Select revolution method 'By angles'
    clickButton(waitForObject(":Revolution_QToolButton_2"))
    
    #[step] Rotate the model
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 275, 421, -54, -250, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 701, 177, -272, 109, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 353, 236, -204, -12, 67108866, Qt.RightButton)
    
    #[step] Define angle 'To'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_3"), 41, 11, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox_2"), 60)
    
    #[check] Check that  preview is updated
    test.vp("VP1")
    
    #[step] Set angle 'To'=0
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_3"), 31, 9, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox_2"), "<Keypad_0>")
    
    #[step] Define angle 'From'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_4"), 26, 13, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox_2"),350)

    #[step] Fit all
    fit_all()
    
    #[check] Check that preview is updated
    test.vp("VP2")
    
    #[step] Approve revolutionFuse operation
    clickButton(waitForObject(":RevolutionFuse.property_panel_ok_QToolButton"))
    
    #[check] Check that operation has been executed successfully
    test.vp("VP3")

    # [step] Close application without saving
    close_application()
