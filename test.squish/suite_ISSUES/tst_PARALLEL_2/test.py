def sketch():
    #line creation
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 123, 417, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 175, 132, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 477, 63, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 739, 397, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 222, 334, 0, Qt.LeftButton)
    type(waitForObject(":OpenParts*_AppElements_ViewPort"), "<Esc>")
    
    parallel_create((165, 190), (581, 197))
    parallel_create((285, 135), (469, 364))
    test.vp("VP1")
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 663, 348, -375, 114, 1, Qt.LeftButton)
    test.vp("VP2")
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 301, 125, 284, 67, 1, Qt.LeftButton)
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 156, 249, -15, 184, 1, Qt.LeftButton)
    test.vp("VP3")
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 336, 302, 0, Qt.LeftButton)

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("GeomApp")
    set_defaults()
    
    sketch_create(help_points("XY_plane"), lambda: sketch())

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
