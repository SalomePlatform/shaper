def create_new_document():
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    clickButton(waitForObject(":Activate module.New_QPushButton"))
    
def close_application():
    sendEvent("QCloseEvent", waitForObject(":SALOME*_STD_TabDesktop"))
    clickButton(waitForObject(":Exit.Shutdown servers_QCheckBox"))
    clickButton(waitForObject(":Exit.Ok_QPushButton"))
    clickButton(waitForObject(":Close active study.Close w/o saving_QPushButton"))