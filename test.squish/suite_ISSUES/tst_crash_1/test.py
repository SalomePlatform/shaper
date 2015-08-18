def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    part_create()
    
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    
    clickButton(waitForObject(":Extrusion.ExtrusionFuse_AppElements_Button"))

    clickButton(waitForObject(":Extrusion.OK_QPushButton"))
    
    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
