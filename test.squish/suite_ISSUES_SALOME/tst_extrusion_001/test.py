def main():
    #[project] NewGEOM
    #[Scenario] Extrusion_001
    #[Topic] 'Extrusion' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_extrusion_001.hdf'
    open(DATA_PATH + "/for_extrusion_001.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME 7.6.0.NewGeom_QToolButton"))
    
    #[step] Activate Part_1   
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree_2"), "Part\\_1", 44, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Execute extrusion by sizes, use Sketch_001, 50 as to size and 10 as from size 
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "Extrusion"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 345, 226, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit"), 25, 5, 0, Qt.LeftButton)
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox"), "<Keypad_5>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox"), "<Keypad_0>")
    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit_2"), 21, 9, 0, Qt.LeftButton)
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), "<Keypad_1>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), "<Keypad_0>")
    
    #[check] Check that preview appears
    test.vp("VP1")
    
    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))
    
    #[check] Check that extrusion has been implemented
    test.vp("VP2")
    
    # [step] Close application without saving
    close_application()
