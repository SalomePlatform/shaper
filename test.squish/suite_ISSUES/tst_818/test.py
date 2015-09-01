def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    #[step] Create parameter 'a = 100'
    parameter_create("a", "100")
    
    #[step] Create sketch with line (0,0) - (a, 0)
    sketch_create(help_points("XY_plane"), lambda: line_create((0, 0), ("a", 0)))

    #[step] Activate context menu for parameter 'a' and click 'Delete'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1)")
    clickItem(":Object browser_XGUI_DataTree", "Parameters (1)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1).a = 100")
    clickItem(":Object browser_XGUI_DataTree", "Parameters (1).a = 100", 10, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).a = 100", 10, 10, 0)
    
    activateItem(waitForObjectItem(":_QMenu", "Delete"))
    
    #[check] Message box with "Selected features are used in the following features: SketchLine_1. These features will be deleted.\nAlso these features will be deleted: Sketch_1.\nWould you like to continue?" should appear
    waitFor("object.exists(':Delete features.Label_QLabel')", 20000)
    test.compare(str(findObject(":Delete features.Label_QLabel").text), "Selected features are used in the following features: SketchLine_1. These features will be deleted.\nAlso these features will be deleted: Sketch_1.\nWould you like to continue?")

    #[step] Click 'Yes' and close application
    clickButton(waitForObject(":Delete features.Yes_QPushButton"))
