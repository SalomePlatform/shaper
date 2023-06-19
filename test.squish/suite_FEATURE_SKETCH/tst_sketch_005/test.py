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

def sketch():
    #[step] Click 'Create sketch' toolbar button
    #[step] Select XY plane for sketching
    #[step] Sketch - Point
    #[step] Create point via clicking inside viewer
    point_create_in_view((340, 205))
    #[step] Check 'Auxiliary' check-box
    #[step] Create point via clicking inside viewer
    point_create_in_view((489, 380), 1)
    #[step] Finish creation of points via viewer
    #[step] Create point via direct input of coordinates. Use coordinates (50, 60)
    point_create((80, 160))
    #[step] Check 'Auxiliary' check-box
    #[step] Create point via direct input of coordinates. Use coordinates (150, 60)
    point_create((250, 50), 1)
    #[step] Fix positions of 2 points
    point_fixe((340, 205))
    point_fixe((504, 227))
    #[check] Check that 2 points have 'anchor' icons near themselves
    test.vp("VP1")
    #[step] Define distance between fixed and free points
    distance((340, 205), (490, 381), (201, 320))
    # [step] Change distance set
    change_distance((275, 407), 200)
    # [step] Check that distance has been modified, free point moved
    test.vp("VP3")

def main():

    # [project] SHAPER
    # [Scenario] Sketch_005
    #[Topic] Fixed point
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
