
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_012
    #[Topic] 'Length' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_012.hdf'
    open(DATA_PATH + "/for_sketch_012.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))
    
    #[step] Define length for main line
    length((78, 376), (130, 341))
    
    #[step] Define length for auxilliary line
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Length"))
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 435, 443, 331, -417, 1, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 546, 169, 0, Qt.LeftButton)
    #clickButton(waitForObject(":Length.property_panel_ok_QToolButton"))
    clickButton(waitForObject(":Length.property_panel_cancel_QToolButton"))
    
    #[check] Check that lengths have been defined successfully
    test.vp("VP1")

    # [step] Change length of main line to 200
    change_length((125, 336), 200)
    
    #[step] Change length of auxilliary line to 500 
    change_length((542, 169), 500)
    
    #[step] Click 'Fit all' button
    fit_all()

    #[check] Check that modifications have been executed successfully
    test.vp("VP2")
 
    #[step] Close application without saving
    close_application()
