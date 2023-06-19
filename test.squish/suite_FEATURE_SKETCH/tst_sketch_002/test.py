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
    #[step] Create random closed contour
    line_create_in_view((101, 183),(265, 107))
    line_create_in_view((265, 107), (520, 192))
    line_create_in_view((520, 192), (443, 411))
    closing_line_create_in_view((443, 411), (101, 183))

    #[step] Create one more closed contour inside previous, not parallel to it
    line_create_in_view((219, 198),(269, 166))
    line_create_in_view((269, 166), (451, 230))
    line_create_in_view((451, 230), (410, 334))
    closing_line_create_in_view((410, 334), (219, 199))

    #[check] Check that lines are not parallel and there are no icons near lines
    test.vp("VP1")
    #[step] Make according lines parallel
    parallel((394, 151), (361, 199))
    parallel((201, 137), (244, 182))
    parallel((245, 278), (292, 249))
    parallel((430, 286), (482, 299))
    # [check] Check that lines are parallel and there are icons near lines
    test.vp("VP2")

def main():
    # [project] SHAPER
    #[Scenario] Sketch_002
    #[Topic] Parallel objects
    #[Tested functionality]Parallelism between main lines

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Activate SHAPER
    #[step]Click 'New' button
    activate_SHAPER()
    #[section] Creation of main lines and make them parallel

    sketch_create(help_points("XY_plane"), lambda: sketch())
    #[step] Save study
    save("RESULTS_PATH + sketch_002")
    if object.exists(":Warning.Yes_QPushButton"):
        clickButton(waitForObject(":Warning.Yes_QPushButton"))
    ##[step] Close application
    close_application()
