def main():
    # [project] NewGEOM
    # [Scenario] 'Platine' model created by PythonAPI
    # [Topic] PythonAPI functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Activate Shaper module and create new file
    activate_newgeom()

    # [step] In Python console type: import examples.Platine
    type(waitForObject(":Python Console_PyConsole_EnhEditor"), "import examples.Platine")
    type(waitForObject(":Python Console_PyConsole_EnhEditor"), "<Return>")
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))

    # [step] Check that the model is correct [vp VP_PLATINE]
    test.vp("VP_PLATINE")

    # [step] Check that the object browser contains all objects from the model [vp VP_TREE]
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (3)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (3)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Constructions (3)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Constructions (3)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Bodies (1)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Bodies (1)", -10, 10, 0, Qt.LeftButton)
    test.vp("VP_TREE")

    # [step] Close application without saving
    close_application()
