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