# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
    source(findFile("scripts", "common.py"))

    startApplication("linux_run.sh")
    set_defaults()

    #[step] Create parameter a=100
    parameter_create("a", "100")
    #[step] Create new parts
    part_create()
    #[step] Create sketch with circle center=(0,0) radius="a"
    sketch_create(help_points("XY_plane"), lambda: circle_create(0, 0, "a"))
    #[step] Crete extrusion with the circle and to_size="a"
    points = [(313, 336)] # circle
    extrusion_feature(points, "a")
    #[step] Close application
    close_application()