# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        TestAPI_Symmetry.py
# Created:     15 Nov 2016
# Author:      Clarisse Genrault (CEA)
from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Dir as direction
from GeomAPI import GeomAPI_Ax1 as axis
from GeomAPI import GeomAPI_Ax2 as plane

# Create boxes
Box_1 = shaperpy.makeBox(10., 10., 10.)
Box_2 = shaperpy.makeBox(10., 10., 10.)
Box_3 = shaperpy.makeBox(10., 10., 10.)
Box_4 = shaperpy.makeBox(10., 10., 10.)
Box_5 = shaperpy.makeBox(10., 10., 10.)
Box_6 = shaperpy.makeBox(10., 10., 10.)
Box_7 = shaperpy.makeBox(10., 10., 10.)
Box_8 = shaperpy.makeBox(10., 10., 10.)
Box_9 = shaperpy.makeBox(10., 10., 10.)
Box_10 = shaperpy.makeBox(10., 10., 10.)
Box_11 = shaperpy.makeBox(10., 10., 10.)
Box_12 = shaperpy.makeBox(10., 10., 10.)
Box_13 = shaperpy.makeBox(10., 10., 10.)
Box_14 = shaperpy.makeBox(10., 10., 10.)
Box_15 = shaperpy.makeBox(10., 10., 10.)
Box_16 = shaperpy.makeBox(10., 10., 10.)
Box_17 = shaperpy.makeBox(10., 10., 10.)
Box_18 = shaperpy.makeBox(10., 10., 10.)

# Create points
origin = pnt(0., 0., 0.)
pnt1 = pnt(-10., -10., -10)

# Create Axis
dirZ = direction(0., 0., 10.)
axZ = axis(origin, dirZ)

dir1 = direction(10., 10., 10.)
ax1 = axis(origin, dir1)

# Create Planes
planeXOY = plane(origin, dirZ)

Symmetry_1 = shaperpy.makeSymmetry(Box_1, origin)
Symmetry_2 = shaperpy.makeSymmetry(Box_2, pnt1)

try :
    Symmetry_6 = shaperpy.makeSymmetry(Box_2, None)
except myExcept,ec :
    assert(ec.what() == "Symmetry builder :: point is not valid.")

Symmetry_7 = shaperpy.makeSymmetry(Box_7, axZ)
Symmetry_8 = shaperpy.makeSymmetry(Box_8, ax1)
Symmetry_13 = shaperpy.makeSymmetry(Box_13, planeXOY)
