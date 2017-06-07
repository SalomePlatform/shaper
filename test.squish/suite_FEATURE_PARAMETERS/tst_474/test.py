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

#[tested functionality] 1. Add parameter 2. Calculate parameter 3. Delete parameter

def main():
    source(findFile("scripts", "common.py"))

    startApplication("salome_run.sh")

    activate_SHAPER()

    #[step] Create parameter 'aa = 4'
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Parameter"))
    type(waitForObject(":Parameter_QLineEdit"), "aa")
    type(waitForObject(":Parameter_ExpressionEditor"), "4")
    type(waitForObject(":Parameter_ExpressionEditor"), "<Return>")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))

    # check the new parameter
    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1).aa = 4")

    clickButton(waitForObject(":SALOME*.New part_QToolButton"))

    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Parameter"))
    type(waitForObject(":Parameter_QLineEdit"), "bb")
    type(waitForObject(":Parameter_ExpressionEditor"), "aa+1")
    type(waitForObject(":Parameter_ExpressionEditor"), "<Return>")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))

    # check the new parameter
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (1).bb = 5")

    openContextMenu(waitForObject(":Object browser.Part set_XGUI_ActiveDocLbl"), 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).aa = 4", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Delete"))
    waitFor("object.exists(':Delete features._QLabel')", 20000)

    an_expected = """Selected parameters are used in the following features: bb.
These features will be deleted.
Or parameters could be replaced by their values.
Would you like to continue?"""
    test.compare(str(findObject(":Delete features._QLabel").text), an_expected)
    clickButton(waitForObject(":Delete features.No_QPushButton"))

    # nothing is deleted
    waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1).aa = 4")
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (1).bb = 5")

    close_application()
