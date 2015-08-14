def sketch():
    
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 256, 202, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 256, 462, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 472, 466, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 467, 205, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 256, 202, 0, Qt.LeftButton)

    perpendicular_create((354, 204),(469, 309))
    perpendicular_create((470, 314),(357, 464))
    perpendicular_create((354, 463),(255, 321))
    
    horizontal_create((358, 203))
    
    distance_create((256, 205), (473, 202), (446, 141), 330)
    distance_create((258, 205), (256, 470), (208, 459), 400)

def sketch_1():    

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 365, 324, 0, Qt.LeftButton)
    
    circle_create_in_view((265, 190), (278, 205))
    circle_create_in_view((460, 194), (476, 209))
    circle_create_in_view((264, 465), (272, 487))
    circle_create_in_view((465, 467), (472, 482))
    
    radius_create((282, 203), (299, 177), 15)
    radius_create((456, 174), (402, 157), 15)
    radius_create((284, 454), (304, 438), 15)
    radius_create((467, 484), (472, 486), 15)
    
    distance_create((269, 191), (463,198), (433, 101), 170)
    distance_create((265, 463), (445, 404), (328, 545), 170)
    distance_create((223, 193), (220, 483), (153, 457), 240)
    distance_create((496, 196), (467, 394), (564, 375), 240)
    
    distance_create((187, 645), (202, 532), (168, 560), 80)
    distance_create((113, 598), (199, 517), (197, 424), 80)
    distance_create((509, 64), (643, 63), (579, 160), 80)
    distance_create((512, 62), (529, 3), (443, 119), 80)

def sketch_2():  
    
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 365, 324, 0, Qt.LeftButton)
    
    line_create_in_view((112, 185), (643, 193))
    line_create_in_view((643, 193), (641, 288))
    line_create_in_view((641, 288), (111, 285))
    line_create_in_view((111, 285), (112, 185))
    
    horizontal_create((344, 187))
    horizontal_create((349, 286))

                
def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    part_create()
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1", 46, 8, 0, Qt.LeftButton)
    
    sketch_create(help_points("XY_plane"), lambda: sketch())
    
    #extrusion
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Extrusion.Extrusion_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 295, 253, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit"), 28, 8, 0, Qt.LeftButton)
    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), "50")
    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit_2"), 29, 10, 0, Qt.LeftButton)
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), "50")
    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))

    #extrusion_feature((361, 391), 50, 50)
    #sketch_create(help_points("XY_plane"), lambda: sketch_1())
    
    extrusion_cut_by_sizes_feature((365, 324), lambda: sketch_1(), 100, 150)
    extrusion_fuse_by_sizes_feature((185, 213), lambda: sketch_2(), 150, 0)
    
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 89, 518, 13, -86, 67108865, Qt.LeftButton)
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 181, 514, -21, -144, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 662, 541, -368, -221, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 609, 270, -48, 320, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 483, 423, 123, 11, 67108866, Qt.RightButton)
    
    clickButton(waitForObject(":OpenParts*.Fit all_QToolButton"))
    test.vp("VP1")#Detail in axonometric view
    
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 391, 374, 0, Qt.LeftButton)
    test.vp("VP2")#Detail is selected

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
