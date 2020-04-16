# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    #[Scenario] Sketch_012
    #[Topic] 'Length' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_012.hdf'
    open(DATA_PATH + "/for_sketch_012.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    #[step] Define length for main line
    length((78, 376), (130, 341))

    #[step] Define length for auxilliary line
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Length"))
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 435, 443, 331, -417, 1, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 546, 169, 0, Qt.LeftButton)
    #clickButton(waitForObject(":Length.property_panel_ok_QToolButton"))
    clickButton(waitForObject(":Length.property_panel_cancel_QToolButton"))

    #[check] Check that lengths have been defined successfully
    test.vp("VP1")

    # [step] Change length of main line to 200
    change_length((125, 336), 200)

    #[step] Change length of auxilliary line to 500
    change_length((542, 169), 500)

    #[step] Click 'Fit all' button
    fit_all()

    #[check] Check that modifications have been executed successfully
    test.vp("VP2")

    #[step] Close application without saving
    close_application()
