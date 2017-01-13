# Copyright (C) 2014-201x CEA/DEN, EDF R&D

# File:        TestAPI_Cylinder.py
# Created:     13 Jan 2017
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAlgoAPI import GeomAlgoAPI_EdgeBuilder as edgeBuilder
from GeomAPI import GeomAPI_Pnt as pnt

aPoint = pnt(10.,5.,0.)
anEdge = edgeBuilder.line(1.,0.,0.)

# Create a cylinder
try :    
  cylinder1 = shaperpy.makeCylinder(5., 20.)
  
except myExcept,ec:
  print ec.what()

# Create a cylinder
try :    
  cylinder2 = shaperpy.makeCylinder(5., 20., 100.)
  
except myExcept,ec:
  print ec.what()
  
# Create a cylinder
try :
  cylinder3 = shaperpy.makeCylinder(aPoint, anEdge, 5., 20.)
  
except myExcept,ec:
  print ec.what()
  
# Create a cylinder
try :
  cylinder4 = shaperpy.makeCylinder(aPoint, anEdge, 5., 20., 180.)
  
except myExcept,ec:
  print ec.what()