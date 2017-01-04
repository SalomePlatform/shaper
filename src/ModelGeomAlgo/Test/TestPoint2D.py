"""
      TestPoint2D.py
      Unit test for testing the Point2D algorithms

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
from ModelGeomAlgo import *

__updated__ = "2016-07-20"

aSession = ModelAPI_Session.get()

from salome.shaper import model
assert(model.checkPythonDump())
