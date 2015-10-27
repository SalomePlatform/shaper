"""Package for Features plugin for the Parametric Geometry API of the Modeler.
"""

from boolean   import Addition        as addAddition
from boolean   import Subtraction     as addSubtraction
from boolean   import Intersection    as addIntersection

from extrusion import addExtrusion
from revolution import addRevolution
from partition import addPartition
