# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        TestAPI_Box.py
# Created:     16 Sept 2016
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

# Create a box with dimensions    
try :    
  box1 = shaperpy.makeBox(10.,10.,10.)
  print "box1 : ok"
  
except myExcept,ec:
  print "box1 : ko (" + ec.what() + ")"
  
try :    
  box2 = shaperpy.makeBox(0.,10.,10.)
  print "box2 : ok"
  
except myExcept,ec:
  print "box2 : ko (" + ec.what() + ")"

try :    
  box3 = shaperpy.makeBox(10.,0.,10.)
  print "box3 : ok"
  
except myExcept,ec:
  print "box3 : ko (" + ec.what() + ")"
  
try :    
  box4 = shaperpy.makeBox(10.,10.,0.)
  print "box4 : ok"
  
except myExcept,ec:
  print "box4 : ko (" + ec.what() + ")"
  
try :    
  box5 = shaperpy.makeBox(-10.,10.,10.)
  print "box5 : ok"
  
except myExcept,ec:
  print "box5 : ko (" + ec.what() + ")"
  
try :    
  box6 = shaperpy.makeBox(10.,-10.,10.)
  print "box6 : ok"
  
except myExcept,ec:
  print "box6 : ko (" + ec.what() + ")"
  
try :    
  box7 = shaperpy.makeBox(10.,10.,-10.)
  print "box7 : ok"
  
except myExcept,ec:
  print "box7 : ko (" + ec.what() + ")"

# Create a box with two points defining the diagonal   
try :    
  pnt1 = pnt(0.,0.,0.)
  pnt2 = pnt(50.,50.,50.)
  box8 = shaperpy.makeBox(pnt1,pnt2)
  print "box8 : ok"
  
except myExcept,ec:
  print "box8 : ko (" + ec.what() + ")"

try :    
  pnt1 = pnt(0.,0.,0.)
  box9 = shaperpy.makeBox(pnt1,pnt1)
  print "box9 : ok"
  
except myExcept,ec:
  print "box9 : ko (" + ec.what() + ")"
  
try :    
  pnt1 = pnt(0.,0.,0.)
  pnt2 = pnt(0.,50.,50.)
  box10 = shaperpy.makeBox(pnt1,pnt2)
  print "box10 : ok"
  
except myExcept,ec:
  print "box10 : ko (" + ec.what() + ")"
  
try :    
  pnt1 = pnt(0.,0.,0.)
  pnt2 = pnt(50.,0.,50.)
  box11 = shaperpy.makeBox(pnt1,pnt2)
  print "box11 : ok"
  
except myExcept,ec:
  print "box11 : ko (" + ec.what() + ")"
  
try :    
  pnt1 = pnt(0.,0.,0.)
  pnt2 = pnt(50.,50.,0.)
  box12 = shaperpy.makeBox(pnt1,pnt2)
  print "box12 : ok"
  
except myExcept,ec:
  print "box12 : ko (" + ec.what() + ")"
  
try :    
  pnt1 = pnt(50.,50.,50.)
  box13 = shaperpy.makeBox(pnt1,None)
  print "box13 : ok"
  
except myExcept,ec:
  print "box13 : ko (" + ec.what() + ")"
  
try :    
  pnt2 = pnt(50.,50.,50.)
  box14 = shaperpy.makeBox(None,pnt2)
  print "box14 : ok"
  
except myExcept,ec:
  print "box14 : ko (" + ec.what() + ")"
  
