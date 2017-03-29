# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        TestAPI_Translation.py
# Created:     15 Nov 2016
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Ax1 as axis
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Dir as direction

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
except myExcept,ec:
    assert(ec.what() == "Translation builder :: axis is not valid.")

Translation_9 = shaperpy.makeTranslation(Box_5, ax4, 15.)

MultiTranslation_1 = shaperpy.makeMultiTranslation(Box_6, ax1, 15., 3, ax2, 15., 3)
Translation_10 = shaperpy.makeTranslation(MultiTranslation_1, ax3, 15.)

MultiTranslation_2 = shaperpy.makeMultiTranslation(Box_7, ax1, 15., 3, ax2, 15., 3)
Translation_11 = shaperpy.makeTranslation(MultiTranslation_2, ax3, 0.)

MultiTranslation_3 = shaperpy.makeMultiTranslation(Box_8, ax1, 15., 3, ax2, 15., 3)
Translation_12 = shaperpy.makeTranslation(MultiTranslation_3, ax3, -15.)

MultiTranslation_4 = shaperpy.makeMultiTranslation(Box_9, ax1, 15., 3, ax2, 15., 3)
try:
    Translation_13 = shaperpy.makeTranslation(MultiTranslation_4, None, 15.)
except myExcept,ec:
    assert(ec.what() == "Translation builder :: axis is not valid.")

MultiTranslation_5 = shaperpy.makeMultiTranslation(Box_10, ax1, 15., 3, ax2, 15., 3)
Translation_18 = shaperpy.makeTranslation(MultiTranslation_5, ax4, 15.)

# Translations "By dimensions in X, in Y and in Z"
Translation_91 = shaperpy.makeTranslation(Box_11, 10., 20., 15.)
Translation_92 = shaperpy.makeTranslation(Box_11, 0., 20., 15.)
Translation_93 = shaperpy.makeTranslation(Box_11, 10., 0., 15.)
Translation_94 = shaperpy.makeTranslation(Box_11, 10., 20., 0.)
Translation_95 = shaperpy.makeTranslation(Box_11, -10., 20., 15.)
Translation_96 = shaperpy.makeTranslation(Box_11, 10., -20., 15.)
Translation_97 = shaperpy.makeTranslation(Box_11, 10., 20., -15.)

# Translations "By two points"
Translation_99 = shaperpy.makeTranslation(Box_11, pnt1, pnt2)

try:
    Translation_100 = shaperpy.makeTranslation(Box_11, pnt1, pnt1)
except myExcept,ec:
    assert(ec.what() == "Translation builder :: start point and end point coincide.")

try:
    Translation_101 = shaperpy.makeTranslation(Box_11, None, pnt1)
except myExcept,ec:
    assert(ec.what() == "Translation builder :: start point is not valid.")

try:
    Translation_102 = shaperpy.makeTranslation(Box_11, pnt1, None)
except myExcept,ec:
    assert(ec.what() == "Translation builder :: end point is not valid.")