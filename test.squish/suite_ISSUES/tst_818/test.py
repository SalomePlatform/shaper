def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    parameter_create("a", "100")
    
    sketch_create(help_points("XY_plane"), lambda: line_create((0, 0), ("a", 0)))

    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1)")
    clickItem(":Object browser_XGUI_DataTree", "Parameters (1)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1).a = 100")
    clickItem(":Object browser_XGUI_DataTree", "Parameters (1).a = 100", 10, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).a = 100", 10, 10, 0)
    
    activateItem(waitForObjectItem(":_QMenu", "Delete"))
    
    waitFor("object.exists(':Delete features.Label_QLabel')", 20000)
    test.compare(str(findObject(":Delete features.Label_QLabel").text), "Selected features are used in the following features: SketchLine_1. These features will be deleted.\nAlso these features will be deleted: Sketch_1.\nWould you like to continue?")

    clickButton(waitForObject(":Delete features.Yes_QPushButton"))
