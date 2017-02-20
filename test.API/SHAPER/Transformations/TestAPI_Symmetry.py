# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        TestAPI_Symmetry.py
# Created:     15 Nov 2016
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Ax1 as axis
from GeomAPI import GeomAPI_Ax2 as plane
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Dir as direction

# Create a box
try :
  box1 = shaperpy.makeBox(10.,10.,10.)
  box2 = shaperpy.makeBox(10.,10.,10.)
  box3 = shaperpy.makeBox(10.,10.,10.)
 
except myExcept, ec:
  print ec.what()

# Perfom a symmetry by a point.
try :
  origin = pnt(0.,0.,0.)
  symmetry1 = shaperpy.makeSymmetry(box1,origin)
 
except myExcept, ec:
  print ec.what()

# Perfom a symmetry by an axis.
try :
  xDir = direction(1.,0.,0.)
  xAxis = axis(origin, xDir)
  translation2 = shaperpy.makeSymmetry(box2,xAxis)
 
except myExcept, ec:
  print ec.what()

# Perfom a symmetry by a plane.
try :
  pnt1 = pnt(0.,0.,10.)
  pnt2 = pnt(10.,0.,10.)
  aPlane = plane(origin, xDir)
  translation3 = shaperpy.makeSymmetry(box3,aPlane)
 
except myExcept, ec:
  print ec.what()
