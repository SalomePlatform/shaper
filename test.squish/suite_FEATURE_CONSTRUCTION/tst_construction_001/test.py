def main():
    # [project] SHAPER
    # [Scenario] Construction_001
    # [Topic] 'Construction of point, axis and plane' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_construction_001.hdf'
    open(DATA_PATH + "/for_construction_001.hdf")

    # [step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.SHAPER_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    # [step] Fit all
    fit_all()
    test.vp("INIT")

    # [step] Create plane basing on upper face of the box
    plane((340, 120), 100)

    # [step] Fit all
    fit_all()

    # [check] Check that plane has been created [vp PLANE]
    test.vp("PLANE")

    # [step] Create 2 points by property panel
    point(-50, -50, 125)
    point(50, 50, 125)

    # [step] Fit all
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))

    # [check] Check that the points have been created [vp POINTS]
    test.vp("POINTS")

    # [step] Create axis between the 2 points
    axis((213, 142), (538, 142))

    # [check] Check that the axis have been created [vp AXIS]
    test.vp("AXIS")

    # [step] Close application without saving
    close_application()
