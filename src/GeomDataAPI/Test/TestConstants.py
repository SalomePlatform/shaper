#===============================================================================
# Test GeomDataAPI static methods
#===============================================================================
from GeomDataAPI import *
assert (GeomDataAPI_Point.typeId() == "Point")
assert (GeomDataAPI_Dir.typeId() == "Dir")  
assert (GeomDataAPI_Point2D.typeId() == "Point2D")  