def main():
    #[project] NewGEOM
    #[Scenario] Revolution_001
    #[Topic] 'Revolution by angles' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_revolution_001.hdf'
    open(DATA_PATH + "/for_revolution_001.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Not loaded Part\\_1", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    
    #[step] Fit all
    fit_all()
    

    # [step] Execute revolution by angles 
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "Revolution"))
    
    #[step] Click '-OZ' button
    clickButton(waitForObject(":SALOME*.+OY_QToolButton"))
    
    #[step] Fit all
    fit_all()

    # [step] Select skecth for revolution
    mouseClick(waitForObject(":Revolution.Select a sketch face_QListWidget"), 77, 38, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 440, 221, 0, Qt.LeftButton)
    # [step] Select vector
    mouseClick(waitForObject(":Revolution_QLineEdit"), 116, 11, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"),306, 399, 0, Qt.LeftButton)
    
    #[step] Input angle 'To'
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), 90)
    
    #[step] Input angle 'From'
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), 120)
    
    #[step] Rotate the model
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 546, 182, -154, 50, 67108866, Qt.RightButton)
   
    #[step] Fit all
    fit_all()
    
    #[check] Check that preview is updated
    test.vp("VP1")
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit"), 58, 8, 0, Qt.LeftButton)

    # [step] Input new angle 'To'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit"), 58, 8, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), "<Delete>")

    #[step] Input new angle 'From'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_2"), 52, 12, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), 360)

    #[step] Ok
    clickButton(waitForObject(":Revolution.property_panel_ok_QToolButton"))
   
    #[step] Fit all
    fit_all()
    
    #[check] Check that operation has been executed successfully
    test.vp("VP2")
   
    # [step] Close application without saving
    close_application()
