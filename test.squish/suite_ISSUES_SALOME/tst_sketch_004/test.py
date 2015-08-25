import os
    
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_004
    #[Topic] 'Distance' constraint
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_004.hdf'
    open(os.path.join(DATA_PATH, "for_sketch_004.hdf"))
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1", 51, 7, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1", 51, 7, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Edit Sketch_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1", 54, 11, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Sketch\\_1", 54, 11, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OY' view button
    clickButton(waitForObject(":SALOME*.+OY_QToolButton"))
    
    #[step] Set constraint 'Distance' between circle and point
    distance((44, 224), (601, 259), (561, 195))
    #[check] Check that constraint has been set
    test.vp("VP1")
    #[step] Change distance to 500
    change_distance((324, 176), 500)
    #[check] Check that distance is 500
    test.vp("VP2")    
    #[step] Delete constraint
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 302, 270, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 326, 181, 0, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Del>")
    #[check] Check that constraint has been removed
    test.vp("VP3")
    #[step] Close application

    close_application()
    
    #[section]
    #[step]
    #[step]
    #[comment]
    #[check]

