# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        TestAPI_Box.py
# Created:     29 Nov 2016
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

# Create several "cone segment"
try :    
  CS1 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,0.,270.)
except myExcept,ec:
  print ec.what()
  
try :    
  CS2 = shaperpy.makeConeSegment(5.,10.,7.,15.,15.,45.,180.)
except myExcept,ec:
  print ec.what()

try :    
  CS3 = shaperpy.makeConeSegment(5.,10.,7.,15.,15.,-45.,180.)
except myExcept,ec:
  print ec.what()
  
try :    
  CS4 = shaperpy.makeConeSegment(0.,10.,2.,7.,3.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS5 = shaperpy.makeConeSegment(5.,8.,0.,6.,10.,45.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS6 = shaperpy.makeConeSegment(0.,6.,0.,4.,17.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS7 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,0.,360.)
except myExcept,ec:
  print ec.what()

try :    
  CS8 = shaperpy.makeConeSegment(-5.,11.,5.,8.,12.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS9 = shaperpy.makeConeSegment(11.,7.,5.,8.,12.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS10 = shaperpy.makeConeSegment(7.,11.,8.,5.,12.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS11 = shaperpy.makeConeSegment(7.,11.,-3.,5.,12.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS12 = shaperpy.makeConeSegment(7.,11.,5.,8.,0.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS13 = shaperpy.makeConeSegment(7.,11.,5.,8.,-10.,0.,270.)
except myExcept,ec:
  print ec.what()

try :    
  CS14 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,-45.,-45.)
except myExcept,ec:
  print ec.what()

try :    
  CS15 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,45.,450.)
except myExcept,ec:
  print ec.what()