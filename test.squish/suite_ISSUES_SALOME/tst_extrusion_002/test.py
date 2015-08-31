def main():
    #[project] NewGEOM
    #[Scenario] Extrusion_002
    #[Topic] 'Extrusion by boarding planes and offsets' functionality
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
    open(DATA_PATH + "/for_extrusion_002.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1", 48, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Not loaded Part\\_1", 48, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Fit all
    fit_all()
    #[step] Execute extrusion for sketch_1
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "Extrusion"))
    
    #[step] Select sketch_1 as base
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 324, 222, 0, Qt.LeftButton)
    
    #[step] Select 'By boarding planes and offsets' button
    clickButton(waitForObject(":Extrusion_QToolButton"))
    
    #[check] Check that no result is previewed
    test.vp("VP1")
    
    #[step] Select upper face as face 'To'
    mouseClick(waitForObject(":Extrusion.Select a sketch face_QListWidget"), 80, 47, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 350, 231, 0, Qt.LeftButton)
    mouseClick(waitForObject(":To_QLineEdit"), 31, 11, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 369, 87, 0, Qt.LeftButton)
    
    #[check] Check that preview is updated
    test.vp("VP2")
    
    #[step] Select lower face as face 'From'
    mouseClick(waitForObject(":From_QLineEdit"), 80, 11, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 350, 413, 0, Qt.LeftButton)
    
    #[check] Check that preview is updated
    test.vp("VP3")
    
    #[step] Define offset for face 'To'
    type(waitForObject(":To.to_offset_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":To.to_offset_ModuleBase_ParamSpinBox"), 200)

    #[step] Fit all
    fit_all()
    
    #[check] Check that preview is updated
    test.vp("VP4")
    
    #[step] Define offset for face 'From'
    type(waitForObject(":From.from_offset_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":From.from_offset_ModuleBase_ParamSpinBox"), 200)

    #[step] Fit all
    fit_all()    

    #[check] Check that preview is updated
    test.vp("VP5")
    
    #[step] Ok
    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))
    
    #[step] Fit all
    fit_all()   
    
    #[check] Check that extrusion is built
    test.vp("VP6")

    # [step] Close application without saving
    close_application()
