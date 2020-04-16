# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

import math

from GeomAPI import *

TOLERANCE = 1.e-7

origin = GeomAPI_Pnt(10, 0, 0)
dir = GeomAPI_Dir(1, 0, 0)

# Test GeomAPI_Ax1
ax1 = GeomAPI_Ax1()
ax1.setOrigin(origin)
ax1.setDir(dir)
assert(ax1.origin().distance(origin) < TOLERANCE)
assert(math.fabs(ax1.dir().dot(dir) - 1.) < TOLERANCE)

ax1.reverse()
ax1 = ax1.reversed()
assert(ax1.origin().distance(origin) < TOLERANCE)
assert(math.fabs(ax1.dir().dot(dir) - 1.) < TOLERANCE)


# Test GeomAPI_Ax2
ax2 = GeomAPI_Ax2()
ax2.setOrigin(origin)
ax2.setDir(dir)
assert(ax2.origin().distance(origin) < TOLERANCE)
assert(math.fabs(ax2.dir().dot(dir) - 1.) < TOLERANCE)

ax2 = GeomAPI_Ax2(origin, dir)
assert(ax2.origin().distance(origin) < TOLERANCE)
assert(math.fabs(ax2.dir().dot(dir) - 1.) < TOLERANCE)

dx = GeomAPI_Dir(0, 1, 0)
ax2 = GeomAPI_Ax2(origin, dir, dx)
assert(ax2.origin().distance(origin) < TOLERANCE)
assert(math.fabs(ax2.dir().dot(dir) - 1.) < TOLERANCE)


# Test GeomAPI_Ax3
dy = GeomAPI_Dir(0, 0, 1)
ax3 = GeomAPI_Ax3()
ax3.setOrigin(origin)
ax3.setNormal(dy)
ax3.setDirX(dir)
ax3.setDirY(dx)
assert(ax3.origin().distance(origin) < TOLERANCE)
assert(math.fabs(ax3.normal().dot(dy) - 1.) < TOLERANCE)
assert(math.fabs(ax3.dirX().dot(dir) - 1.) < TOLERANCE)
assert(math.fabs(ax3.dirY().dot(dx) - 1.) < TOLERANCE)

ax3 = GeomAPI_Ax3(origin, dx, dir)
assert(ax3.origin().distance(origin) < TOLERANCE)
assert(math.fabs(ax3.normal().dot(dir) - 1.) < TOLERANCE)
assert(math.fabs(ax3.dirX().dot(dx) - 1.) < TOLERANCE)
assert(math.fabs(ax3.dirY().dot(dy) - 1.) < TOLERANCE)

p2d = GeomAPI_Pnt2d(1, 2)
a2d = ax3.to2D(origin.x() + dx.x() * p2d.x() + dy.x() * p2d.y(),
               origin.y() + dx.y() * p2d.x() + dy.y() * p2d.y(),
               origin.z() + dx.z() * p2d.x() + dy.z() * p2d.y())
assert(a2d.distance(p2d) < TOLERANCE)
