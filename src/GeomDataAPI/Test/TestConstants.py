#===============================================================================
# Test GeomDataAPI static methods
#===============================================================================
from GeomDataAPI import *
assert (GeomDataAPI_Point.type() == "Point")
assert (GeomDataAPI_Dir.type() == "Dir")  
assert (GeomDataAPI_Point2D.type() == "Point2D")  