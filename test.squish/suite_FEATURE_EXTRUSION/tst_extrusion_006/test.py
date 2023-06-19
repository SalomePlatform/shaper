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
    # [project] SHAPER
    # [Scenario] Extrusion_006
    # [Topic] 'Extrusion_cut by planes' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_extrusion_006.hdf'
    open(DATA_PATH + "/for_extrusion_006.hdf")

    # [step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    # [step] Fit all
    fit_all()
    test.vp("INIT")

    # [step] Features - ExtrusionCut
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "ExtrusionCut"))

    # [step] Select the upper face for sketching
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 243, 171, 0, Qt.LeftButton)

    # [step] Click "Set plane view" in property panel [vp PLANE]
    clickButton(waitForObject(":Sketcher plane.Set plane view_QPushButton"))
    test.vp("PLANE")

    # [step] Draw closing contour, consisting from 4 lines
    lines_create_in_view([(110, 150), (110, 320), (640, 320), (640, 150), (110, 155)])

    # [check] Check that lines have been executed successfully [cp LINES]
    fit_all()
    test.vp("LINES")

    # [step] Ok
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))

    # [step] Select 'By planes and offsets' method
    clickButton(waitForObject(":Extrusion_QToolButton_3"))
    mouseClick(waitForObject(":To_QLineEdit_5"), 46, 15, 0, Qt.LeftButton)

    # [step] Select Plane_1 as face 'To'
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 35, 220, 0, Qt.LeftButton)

    # [check] Check that preview has been updated [cp PREVIEW]
    fit_all()
    test.vp("PREVIEW")

    # [step] Ok
    clickButton(waitForObject(":ExtrusionCut.property_panel_ok_QToolButton"))

    # [step] Click 'Reset view' toolbar button
    clickButton(waitForObject(":SALOME*.Reset_QToolButton_2"))
    mouseClick(waitForObject(":SALOME*_OCCViewer_ViewPort3d"), 10, 450, 0, Qt.LeftButton)  # close tool bar extension bar

    # [check] Check that extrusionCut operation has been executed successfully [vp EXTRUSION_FUSE]
    test.vp("EXTRUSION_CUT")

    # [step] Close application without saving
    close_application()
