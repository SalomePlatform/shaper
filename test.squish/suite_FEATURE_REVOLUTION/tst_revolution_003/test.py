def main():
    #[project] NewGEOM
    #[Scenario] Revolution_003
    #[Topic] 'RevolutionCut by angles' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_revolution_003.hdf'
    open(DATA_PATH + "/for_revolution_003.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Fit all
    fit_all()
    
    #[step] Execute RevolutionFuse operation
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "RevolutionCut"))
    
    #[step] Select sketch face
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 287, 236, 0, Qt.LeftButton)

    # [step] Create 2 circles
    circle_create_in_view((650, 364), (671, 392))
    circle_create_in_view((641, 114), (664, 141))
    
    #[step] Approve sketching
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
    # [step] Select axis for revolution
    mouseClick(waitForObject(":Revolution_QLineEdit_3"), 103, 11, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 750, 237, 0, Qt.LeftButton)
    
    #[step] Define angle 'To'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_5"), 61, 9, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox_3"), 90)

    #[step] Define angle 'From'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_6"), 104, 13, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox_3"), 90)


    #[step] Ok
    clickButton(waitForObject(":RevolutionCut.property_panel_ok_QToolButton"))
    
    #[step] Rotate the model
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 558, 413, -33, -137, 67108866, Qt.RightButton)
    
    #[check] Check that operation has been executed successfully
    test.vp("VP1")

    # [step] Close application without saving
    close_application()
