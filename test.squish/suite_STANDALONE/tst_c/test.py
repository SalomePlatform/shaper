# Copyright (C) 2014-2023  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

def main():
    source(findFile("scripts", "common.py"))

    startApplication("linux_run.sh")
    set_defaults()

    clickButton(waitForObject(":Operations.New part_AppElements_Button"))

    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Sketch")
    clickButton(waitForObject(":Basic.Sketch_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 351, 267, 0, Qt.LeftButton)
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 242, 169, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 140, 394, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 531, 537, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 241, 169, 0, Qt.LeftButton)
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))

    test.vp("VP_SKETCH")

    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Extrusion.Extrusion_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 216, 270, 0, Qt.LeftButton)
    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))

    test.vp("VP_EXTRUSION")

    clickButton(waitForObject(":Extrusion.ExtrusionFuse_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 290, 316, 0, Qt.LeftButton)

    clickButton(waitForObject(":OpenParts*.Fit all_QToolButton"))
    test.vp("VP_EXTRUSIONFUSE")

    mouseClick(waitForObject(":Sketch.Basic_AppElements_MenuGroupPanel"), 143, 56, 0, Qt.LeftButton)
    clickButton(waitForObject(":Basic.Circle_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 259, 248, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 281, 285, 0, Qt.LeftButton)
    clickButton(waitForObject(":Sketch.Ok_QToolButton"))

    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit_5"), 42, 9, 0, Qt.LeftButton)
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "3")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "3")
    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit_6"), 93, 7, 0, Qt.LeftButton)
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "4")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "4")
    clickButton(waitForObject(":ExtrusionFuse.property_panel_ok_QToolButton"))

    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Sketch")
    clickButton(waitForObject(":Basic.Sketch_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 351, 267, 0, Qt.LeftButton)
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 242, 169, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 140, 394, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 531, 537, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 241, 169, 0, Qt.LeftButton)
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))

    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    mouseDrag(waitForObject(":Features_QScrollArea"), 31, 1, -2, -2, 1, Qt.LeftButton)
    clickButton(waitForObject(":Extrusion.Extrusion_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 216, 270, 0, Qt.LeftButton)
    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))
    clickButton(waitForObject(":Extrusion.ExtrusionCut_AppElements_Button"))
    clickButton(waitForObject(":Basic_QToolButton"))
    clickButton(waitForObject(":Abort operation.OK_QPushButton"))
    clickButton(waitForObject(":Extrusion.ExtrusionFuse_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 290, 316, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Sketch.Basic_AppElements_MenuGroupPanel"), 143, 56, 0, Qt.LeftButton)
    clickButton(waitForObject(":Basic.Circle_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 259, 248, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 281, 285, 0, Qt.LeftButton)
    clickButton(waitForObject(":Sketch.Ok_QToolButton"))

    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit_5"), 42, 9, 0, Qt.LeftButton)
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "33")
    mouseClick(waitForObject(":Extrusion.qt_spinbox_lineedit_QLineEdit_6"), 93, 7, 0, Qt.LeftButton)
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "44")
    clickButton(waitForObject(":ExtrusionFuse.property_panel_ok_QToolButton"))

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
