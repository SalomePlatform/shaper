# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        TestAPI_Translation.py
# Created:     15 Nov 2016
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Ax1 as axis
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Dir as direction

# Create a box
try :
  box = shaperpy.makeBox(10.,10.,10.)
 
except myExcept, ec:
  print ec.what()

# Perfom a translation with an axis and a distance.
try :
  origin = pnt(0.,0.,0.)
  xDir = direction(1.,0.,0.)
  xAxis = axis(origin, xDir)
  translation1 = shaperpy.makeTranslation(box,xAxis,15.)
 
except myExcept, ec:
  print ec.what()

# Perfom a translation with dimensions.
try :
  translation2 = shaperpy.makeTranslation(box,10,0,0)
 
except myExcept, ec:
  print ec.what()
