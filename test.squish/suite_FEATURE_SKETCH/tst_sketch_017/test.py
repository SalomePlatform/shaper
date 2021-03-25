# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
    #[project] SHAPER
    #[Scenario] Sketch_017
    #[Topic] 'Rotation' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_016.hdf'
    open(DATA_PATH + "/for_sketch_016.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    #[step] Sketch - Rotation
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Rotation"))

    #[step] Select all object for rotation
    mouseClick(waitForObject(":Rotation.Segments:_QListWidget"), 59, 66, 0, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Shift>")
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 135, 116, 33554432, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 447, 187, 33554432, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 573, 209, 33554432, Qt.LeftButton)

    #[step] Click on the viewer to select center of rotation
    mouseClick(waitForObject(":Center of rotation.qt_spinbox_lineedit_QLineEdit"), 107, 8, 0, Qt.LeftButton)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 387, 328, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 387, 328, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 387, 328, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 387, 325, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 387, 325, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 387, 325, -120, 0, 2)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 605, 257, 0, Qt.LeftButton)

    #[step] Input angle of rotation
    mouseClick(waitForObject(":Rotation.qt_spinbox_lineedit_QLineEdit"), 146, 5, 0, Qt.LeftButton)
    type(waitForObject(":Rotation.MultiRotationAngle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Rotation.MultiRotationAngle_ModuleBase_ParamSpinBox"), 45)

    #[step] Input number of copies
    mouseClick(waitForObject(":Rotation.qt_spinbox_lineedit_QLineEdit_2"), 13, 5, 0, Qt.LeftButton)
    type(waitForObject(":Rotation.MultiRotationCopies_QSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Rotation.MultiRotationCopies_QSpinBox"), 7)

    mouseClick(waitForObject(":Rotation.Segments:_QListWidget"), 59, 66, 0, Qt.LeftButton)

    #[step] Click Fit all button
    fit_all()

    #[check] Check that preview is updated
    test.vp("VP1")

    #[step] Ok
    clickButton(waitForObject(":Rotation.property_panel_ok_QToolButton"))

    #[check] Check that rotation has been executed successfully
    test.vp("VP2")

    # [step] Close application without saving
    close_application()
