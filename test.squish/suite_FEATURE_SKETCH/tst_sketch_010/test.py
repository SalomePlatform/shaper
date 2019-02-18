# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
    #[Scenario] Sketch_010
    #[Topic] 'Vertical' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_006.hdf'
    open(DATA_PATH + "/for_sketch_010.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    #[step] Make one of lines vertical
    vertical((129, 346))

    #[step] Fit all
    fit_all()

    #[check] Check that one of lines became vertical
    test.vp("VP1")

    #[step] Uncheck 'Show constraint' check-box
    clickButton(waitForObject(":Sketch.Show constraints_QCheckBox"))

    #[check] Check that icon disappears
    test.vp("VP3")

    #[step] Close application without saving
    close_application()
