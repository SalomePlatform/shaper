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

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Ax1 as axis
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Dir as direction
from GeomAPI import GeomAPI_Shape as shape

# Points
pntOrigin = pnt(0.,0.,0.)
pnt1 = pnt(10.,0.,0.)
pnt2 = pnt(10.,10.,0.)

# Axis
xDir = direction(10., 0., 0.)
ax1 = axis(pntOrigin, xDir)

yDir = direction(0., 10., 0.)
ax2 = axis(pntOrigin, yDir)

zDir = direction(0., 0., 10.)
ax3 = axis(pntOrigin, zDir)

dir1 = direction(10., 10., 10.)
ax4 = axis(pntOrigin, dir1)

# Boxes
Box_1 = shaperpy.makeBox(10., 20., 10.)
Box_2 = shaperpy.makeBox(10., 20., 10.)
Box_3 = shaperpy.makeBox(10., 20., 10.)
Box_4 = shaperpy.makeBox(10., 20., 10.)
Box_5 = shaperpy.makeBox(10., 20., 10.)
Box_6 = shaperpy.makeBox(10., 20., 10.)
Box_7 = shaperpy.makeBox(10., 20., 10.)
Box_8 = shaperpy.makeBox(10., 20., 10.)
Box_9 = shaperpy.makeBox(10., 20., 10.)
Box_10 = shaperpy.makeBox(10., 20., 10.)
Box_11 = shaperpy.makeBox(10., 20., 10.)
Box_12 = shaperpy.makeBox(10., 20., 10.)
Box_13 = shaperpy.makeBox(10., 20., 10.)
Box_14 = shaperpy.makeBox(10., 20., 10.)
Box_15 = shaperpy.makeBox(10., 20., 10.)
Box_16 = shaperpy.makeBox(10., 20., 10.)
Box_17 = shaperpy.makeBox(10., 20., 10.)
Box_18 = shaperpy.makeBox(10., 20., 10.)

#Translations "By an axis and a distance"
Translation_1 = shaperpy.makeTranslation(Box_1, ax1, 15.)
Translation_2 = shaperpy.makeTranslation(Box_2, ax1, 0.)
Translation_3 = shaperpy.makeTranslation(Box_3, ax1, -15.)

try:
    Translation_4 = shaperpy.makeTranslation(Box_4, None, 15.)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: axis is not valid.")

Translation_5 = shaperpy.makeTranslation(Box_5, ax4, 15.)

MultiTranslation_1 = shaperpy.makeMultiTranslation(Box_6, ax1, 15., 3, ax2, 15., 3)
Translation_6 = shaperpy.makeTranslation(MultiTranslation_1, ax3, 15.)

MultiTranslation_2 = shaperpy.makeMultiTranslation(Box_7, ax1, 15., 3, ax2, 15., 3)
Translation_7 = shaperpy.makeTranslation(MultiTranslation_2, ax3, 0.)

MultiTranslation_3 = shaperpy.makeMultiTranslation(Box_8, ax1, 15., 3, ax2, 15., 3)
Translation_8 = shaperpy.makeTranslation(MultiTranslation_3, ax3, -15.)

MultiTranslation_4 = shaperpy.makeMultiTranslation(Box_9, ax1, 15., 3, ax2, 15., 3)
try:
    Translation_9 = shaperpy.makeTranslation(MultiTranslation_4, None, 15.)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: axis is not valid.")

MultiTranslation_5 = shaperpy.makeMultiTranslation(Box_10, ax1, 15., 3, ax2, 15., 3)
Translation_10 = shaperpy.makeTranslation(MultiTranslation_5, ax4, 15.)

# Translations "By dimensions in X, in Y and in Z"
Translation_11 = shaperpy.makeTranslation(Box_11, 10., 20., 15.)
Translation_12 = shaperpy.makeTranslation(Box_11, 0., 20., 15.)
Translation_13 = shaperpy.makeTranslation(Box_11, 10., 0., 15.)
Translation_14 = shaperpy.makeTranslation(Box_11, 10., 20., 0.)
Translation_15 = shaperpy.makeTranslation(Box_11, -10., 20., 15.)
Translation_16 = shaperpy.makeTranslation(Box_11, 10., -20., 15.)
Translation_17 = shaperpy.makeTranslation(Box_11, 10., 20., -15.)

# Translations "By two points"
Translation_18 = shaperpy.makeTranslation(Box_11, pnt1, pnt2)

try:
    Translation_19 = shaperpy.makeTranslation(Box_11, pnt1, pnt1)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: start point and end point coincide.")

try:
    Translation_20 = shaperpy.makeTranslation(Box_11, None, pnt1)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: start point is not valid.")

try:
    Translation_21 = shaperpy.makeTranslation(Box_11, pnt1, None)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: end point is not valid.")

try:
    Translation_22 = shaperpy.makeTranslation(None, ax1, 15.)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: source shape is not valid.")

try:
    Translation_23 = shaperpy.makeTranslation(None, 10., 20., 15.)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: source shape is not valid.")

try:
    Translation_24 = shaperpy.makeTranslation(None, pnt1, pnt2)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: source shape is invalid.")

try:
    Translation_25 = shaperpy.makeTranslation(shape(), ax1, 15.)
except myExcept as ec:
    assert(ec.what() == "Translation builder :: source shape does not contain any actual shape.")
