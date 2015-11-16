#[tested functionality] 

ENABLE_COLOR='#FF000000'
DISABLE_COLOR='#FFC0C0C0'

def checkPartActivatedState(scenario):
    #[step] Check that Part is activated
    allPartSetElements = [
                "Parameters (0)", 
                "Constructions (4)", 
                "Constructions (4).Origin", 
                "Constructions (4).YOZ",
                "Constructions (4).XOZ",
                "Constructions (4).XOY",
                "Parts (1)",
                "Parts (1).Part\\_1"]
    for element in allPartSetElements:
        test.compare(DISABLE_COLOR, waitForObjectItem(":Object browser_XGUI_DataTree", element).foregroundColor, "%s is disabled after: %s" % (element, scenario))

    partElements = [
                "Part\\_1", 
                "Part\\_1.Parameters (0)",
                "Part\\_1.Constructions (0)",
                "Part\\_1.Bodies (0)"]
    for element in partElements:
        test.compare(ENABLE_COLOR, waitForObjectItem(":Object browser_XGUI_DataTree", element).foregroundColor, "%s is enabled after: %s" % (element, scenario))
    
    #[step] Check that Part tree is unfolded
    test.compare(False, waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1").collapsed, "Part is unfolded after: %s" % scenario)

def checkPartDeactivatedState(scenario):
    #[step] Check that Part id disabled and folded 
    partElements = [
                "Part\\_1.Parameters (0)",
                "Part\\_1.Constructions (0)",
                "Part\\_1.Bodies (0)"]
    for element in partElements:
        test.compare(DISABLE_COLOR, waitForObjectItem(":Object browser_XGUI_DataTree", element).foregroundColor, "%s is disabled after: %s" % (element, scenario))
    test.compare(ENABLE_COLOR, waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1").foregroundColor, "Part is enable after: %s" % scenario)
    test.compare(True, waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1").collapsed, "Part is unfolded after: %s" % scenario)
    
    #[step] Check that PartSet is enabled  
    allPartSetElements = [
                "Parameters (0)", 
                "Constructions (4)", 
                "Constructions (4).Origin", 
                "Constructions (4).YOZ",
                "Constructions (4).XOZ",
                "Constructions (4).XOY",
                "Parts (1)",
                "Parts (1).Part\\_1"]
    for element in allPartSetElements:
        test.compare(ENABLE_COLOR, waitForObjectItem(":Object browser_XGUI_DataTree", element).foregroundColor, "%s is enabled after: %s" % (element, scenario))

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("salome_run.sh")
    activate_newgeom()

    #[step] Check that default elements are enabled: Parameters (0), Constructions (4), Parts (0)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Constructions (4)")
    clickItem(":Object browser_XGUI_DataTree", "Constructions (4)", -10, 10, 0, Qt.LeftButton)
     
    allPartSetElements = [
                "Parameters (0)", 
                "Constructions (4)", 
                "Constructions (4).Origin", 
                "Constructions (4).YOZ",
                "Constructions (4).XOZ",
                "Constructions (4).XOY",
                "Parts (0)"]
    for element in allPartSetElements:
        test.compare(ENABLE_COLOR, waitForObjectItem(":Object browser_XGUI_DataTree", element).foregroundColor, "%s is enabled by default" % element)

    #[step] Create Part
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "New part"))
     
    checkPartActivatedState("Part creation")
 
    #[step] Deactivate part by double click 
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1")
    doubleClickItem(":Object browser_XGUI_DataTree", "Part\\_1", 10, 10, 0, Qt.LeftButton)

    checkPartDeactivatedState("Part deactivation by double click")

    #[step] Activate part by double click 
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1")
    doubleClickItem(":Object browser_XGUI_DataTree", "Part\\_1", 10, 10, 0, Qt.LeftButton)

    checkPartActivatedState("Part activation by double click")

    #[step] Check that Activate is disabled in Part context menu  
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1", 10, 10, 0)
    test.compare(False, findMenuItem(waitForObject(":_QMenu"), "Activate").enabled, "Activate is disabled on active Part")

    #[step] Deactivate part by context menu on PartSet 
    openContextMenu(waitForObject(":Object browser.Part set_XGUI_ActiveDocLbl"), 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    checkPartDeactivatedState("Part deactivation by context menu on PartSet")

    #[step] Check that Activate is disabled in PartSet context menu  
    openContextMenu(waitForObject(":Object browser.Part set_XGUI_ActiveDocLbl"), 10, 10, 0)
    test.compare(False, findMenuItem(waitForObject(":_QMenu"), "Activate").enabled, "Activate is disabled on active PartSet")

    #[step] Activate part by context menu on Part
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1", 104, 6, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    checkPartActivatedState("Part activation by context menu on Part")

    close_application()
