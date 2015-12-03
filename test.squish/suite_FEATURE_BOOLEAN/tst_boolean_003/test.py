def main():
    # [project] NewGEOM
    # [Scenario] Boolean_003
    # [Topic] 'Boolean common' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_extrusion_001.hdf'
    open(DATA_PATH + "/for_boolean_001.hdf")

    # [step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 48, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 48, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    # [step] Fit all
    fit_all()
    test.vp("INIT")

    # [step] Implement boolean common for existing objects
    boolean_common((139, 138), (420, 195))

    # [check] Check that operation has been executed successfully
    test.vp("COMMON")

    # [step] Close application without saving
    close_application()
