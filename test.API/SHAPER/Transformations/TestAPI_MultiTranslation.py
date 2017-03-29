# Copyright (C) 2014-201x CEA/DEN, EDF R&D

# File:        TestAPI_MultiTranslation.py
# Created:     17 Mar 2017
# Author:      Sylvain Chouteau (Alyotech)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Ax1 as axis, GeomAPI_Pnt as pnt, GeomAPI_Dir as direction

# Create Boxes
Box_1 = shaperpy.makeBox(10.,10.,10.)
Box_2 = shaperpy.makeBox(10.,10.,10.)
Box_3 = shaperpy.makeBox(10.,10.,10.)
Box_4 = shaperpy.makeBox(10.,10.,10.)
Box_5 = shaperpy.makeBox(10.,10.,10.)
Box_6 = shaperpy.makeBox(10.,10.,10.)
Box_7 = shaperpy.makeBox(10.,10.,10.)
Box_8 = shaperpy.makeBox(10.,10.,10.)

# Points
pntOrigin = pnt(0.,0.,0.)
pnt1 = pnt(10.,0.,0.)
pnt2 = pnt(10.,10.,0.)

# Axis
xDir = direction(10., 0., 0.)
ax1 = axis(pntOrigin, xDir)

yDir = direction(0., 10., 0.)
ax2 = axis(pntOrigin, yDir)


# Create MultiTranslations
MultiTranslation_1 = shaperpy.makeMultiTranslation(Box_1, ax1, 15., 5)
MultiTranslation_2 = shaperpy.makeMultiTranslation(Box_2, ax1, -15., 5)

try:
    MultiTranslation_3 = shaperpy.makeMultiTranslation(Box_3, ax1, 15., -2)
except myExcept,ec:
    assert(ec.what() == "Multitranslation builder :: the number of copies for the first direction is null or negative.")

MultiTranslation_5 = shaperpy.makeMultiTranslation(Box_5, ax1, 10., 5, ax2, 10., 5)
MultiTranslation_6 = shaperpy.makeMultiTranslation(Box_6, ax1, 15., 5, ax2, -10., 5)

# Tests en erreur
#try:
#    MultiTranslation_4 = shaperpy.makeMultiTranslation(Box_4, None, 15., 2)
#except myExcept,ec:
#    assert(ec.what() == "Multitranslation builder :: the first axis is not valid.")

# Pas d'exception levee alors qu'une devrait y en avoir une
try:
    MultiTranslation_7 = shaperpy.makeMultiTranslation(Box_7, ax1, 15., 5, ax2, 10., -2)
except myExcept,ec:
    assert(ec.what() == "Multitranslation builder :: the number of copies for the second direction is null or negative.")

#try:
#    MultiTranslation_8 = shaperpy.makeMultiTranslation(Box_8, ax1, 15., 5, None, 10., 5)
#except myExcept,ec:
#    assert(ec.what() == "Multitranslation builder :: the second axis is not valid.")
