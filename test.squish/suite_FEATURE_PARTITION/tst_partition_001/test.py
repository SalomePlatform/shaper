def main():
    # [project] SHAPER
    # [Scenario] Partition_001
    # [Topic] 'Partition' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_partition_001.hdf'
    open(DATA_PATH + "/for_partition_001.hdf")

    # [step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 95, 7, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))

    # [step] Fit all
    fit_all()
    test.vp("VP_FITALL")

    # [step] Create partition with horizontal plane [vp VP_PARTITION_1]
    partition([(290, 170)], (176, 213))

    test.vp("VP_PARTITION_1")

    # [step] Create partition with vertical plane [vp VP_PARTITION_2]
    partition([(260, 165), (295, 366)], (453, 432))

    test.vp("VP_PARTITION_2")

    #[step] Hide one of parts obtained in the viewer: select it, call context menu - Hide
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 291, 198, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 291, 198, 0, Qt.RightButton)
    activateItem(waitForObjectItem(":_QtxMenu", "Hide"))

    # [check] Check that partition has been executed successfully [vp VP_HIDE]
    test.vp("VP_HIDE")

    # [step] Close application without saving
    close_application()
