def sketch():
    circle_create_in_view((302, 221), (436, 287))
    
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_003
    #[Topic]
    #[Tested functionality] Saving and opening studies
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    #[section] Creation of 3D model
    #[step] Activate NewGeom
    #[step]Click 'New' button
    
    activate_newgeom()
    #[step] Create new part
    part_create()
    #[step] Create sketch, consisting from one circle
    sketch_create(help_points("XZ_plane"), lambda: sketch())
    
    #[step] Execute extrusion. Use parameters to size = 100 and from size = 50
    extrusion((182, 140), 100, 50)
    
    #[step] Rotate obtained model
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 425, 422, 20, -23, 67108865, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 446, 411, -13, -137, 67108866, Qt.RightButton)
    test.vp("VP1")

    #[section] Save file
    #[step] File - Save as
    #[step] 'Save as' dialog appears
    #[step] Type name of file
    #[step] Click 'Save' button    
    save("RESULTS_PATH + sketch_003")
    if object.exists(":Warning.Yes_QPushButton"):
        clickButton(waitForObject(":Warning.Yes_QPushButton"))
    
    #[step] Close document
    clickButton(waitForObject(":SALOME*.Close_QToolButton"))
    
    #[step] File - Open 
    #[step} Open just saved file  
    open("RESULTS_PATH + sketch_003.hdf")
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1", 50, 11, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1", 50, 11, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[check] Check that file was loaded correct
    test.vp("VP2")
    #[step] Close application
    close_application()
