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
    #[project] SHAPER
    #[Scenario] Sketch_013
    #[Topic] 'Radius' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_013.hdf'
    open(DATA_PATH + "/for_sketch_013.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    #[step] Define radius for circle
    radius((270, 252), (205, 224))

    #[step] Define radius for arc
    radius((718, 168), (677, 203))

    #[check] Check that constraints have been set successfully
    test.vp("VP1")

    #[step] Change radius of the circle to 150: select constraint and type '150' in 'Radius' field
    change_radius((192, 228), 150)

    #[step] Change radius of the arc to 100: select constraint and type '100' in 'Radius' field
    change_radius((674, 200), 100)

    #[step] Click 'Fit all' button
    fit_all()

    #[check] Check that radiuses have been changed successfully
    test.vp("VP2")

    #[step] Close application without saving
    close_application()
