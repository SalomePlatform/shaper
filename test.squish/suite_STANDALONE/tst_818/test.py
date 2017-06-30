## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

import re

def main():
    source(findFile("scripts", "common.py"))

    startApplication("linux_run.sh")
    set_defaults()

    #[step] Create parameter 'a = 100'
    parameter_create("a", "100")

    #[step] Create sketch with line (0,0) - (a, 0)
    sketch_create(help_points("XY_plane"), lambda: line_create((0, 0), ("a", 0)))

    #[step] Activate context menu for parameter 'a' and click 'Delete'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1)")
    clickItem(":Object browser_XGUI_DataTree", "Parameters (1)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1).a = 100")
    clickItem(":Object browser_XGUI_DataTree", "Parameters (1).a = 100", 10, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).a = 100", 10, 10, 0)

    activateItem(waitForObjectItem(":_QMenu", "Delete"))

    #[check] Message box with "Selected features are used in the following features: SketchLine_1. These features will be deleted.\nAlso these features will be deleted: Sketch_1.\nWould you like to continue?" should appear
    waitFor("object.exists(':Delete features.Label_QLabel')", 20000)

    an_expected = """Selected parameters are used in the following features: SketchLine_1.
These features will be deleted.
(Also these features will be deleted: Sketch_1)
Or parameters could be replaced by their values.
Would you like to continue?"""
    test.compare(str(findObject(":Delete features.Label_QLabel").text), an_expected)

    #[step] Click 'Yes' and close application
    clickButton(waitForObject(":Delete features.Yes_QPushButton"))
