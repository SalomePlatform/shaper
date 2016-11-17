# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        TestAPI_Box.py
# Created:     16 Sept 2016
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

# Create a box with dimensions    
try :    
  box1 = shaperpy.makeBox(5.,15.,5.)
  
except myExcept,ec:
  print ec.what()

# Create a box with two points defining the diagonal   
try :    
  pnt1 = pnt(0.,0.,0.)
  pnt2 = pnt(10.,10.,10.)
  box2 = shaperpy.makeBox(pnt1,pnt2)
  
except myExcept,ec:
  print ec.what()
  
# Create a box with null dimensions
try :    
  box3 = shaperpy.makeBox(0.,0.,0.)
  
except myExcept,ec:
  print ec.what()
  
# Create a box with negative dimensions
try :    
  box4 = shaperpy.makeBox(-5.,15.,5.)
  
except myExcept,ec:
  print ec.what()
  
# Create a box with two same points   
try :    
  pnt1 = pnt(0.,0.,0.)
  box5 = shaperpy.makeBox(pnt1,pnt1)
  
except myExcept,ec:
  print ec.what()