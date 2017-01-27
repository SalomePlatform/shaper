# Copyright (C) 2014-201x CEA/DEN, EDF R&D

# File:        TestAPI_Scale.py
# Created:     25 Jan 2017
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

# Create a box
try :
  box1 = shaperpy.makeBox(10.,10.,10.)
  centerPoint = pnt(20.,20.,0.)
 
except myExcept, ec:
  print ec.what()

# Perfom a scale by a factor.
try :
  scale1 = shaperpy.makeScale(box1,centerPoint, -1.6)
 
except myExcept, ec:
  print ec.what()
