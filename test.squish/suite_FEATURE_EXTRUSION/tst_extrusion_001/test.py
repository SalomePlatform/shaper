# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
    # [Scenario] Extrusion_001
    # [Topic] 'Extrusion from size to size' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_extrusion_001.hdf'
    open(DATA_PATH + "/for_extrusion_001.hdf")

    # [step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 48, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 48, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    # [step] Fit all
    fit_all()
    test.vp("INIT")

    # [step] Execute extrusion for sketch_1
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "Extrusion"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 347, 228, 0, Qt.LeftButton)

    type(waitForObject(getSpinBoxRealName("to_size")), "<Ctrl+A>")
    type(waitForObject(getSpinBoxRealName("to_size")), 50)

    type(waitForObject(getSpinBoxRealName("from_size")), "<Ctrl+A>")
    type(waitForObject(getSpinBoxRealName("from_size")), 10)

    # [check] Check that preview is updated [vp PREVIEW]
    fit_all()
    test.vp("PREVIEW")
    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))

    # [check] Check that extrusion has been implemented [vp EXTRUSION]
    fit_all()
    test.vp("EXTRUSION")

    # [step] Close application without saving
    close_application()
