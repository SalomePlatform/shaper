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

def sketch():
    #[step] Click 'Create sketch' toolbar button
    #[step] Select XY plane for sketching
    #[step] Create point
    point_create_in_view((340, 205))
    #[step] Create line
    line_create_in_view((274, 367), (554, 189))
    #[step] Make them coincident
    coincident((340, 205), (390, 293))
    #[check] Check that objects are coincident
    test.vp("VP1")
    #[step] Uncheck 'Show constraints' check-box
    clickButton(waitForObject(":Sketch.Show constraints_QCheckBox"))
    #[check] Check that 'Coincident' icon disappears
    test.vp("VP2")

def main():

    # [project] SHAPER
    # [Scenario] Sketch_008
    #[Topic] 'Coincident' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]

    source(findFile("scripts", "common.py"))
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Activate SHAPER
    #[step]Click 'New' button
    activate_SHAPER()

    #[section] Creation of sketch and objects inside it
    #[step] Part - New part
    part_create()

    sketch_create(help_points("XY_plane"), lambda: sketch())

    #[step] Close application without saving
    close_application()
