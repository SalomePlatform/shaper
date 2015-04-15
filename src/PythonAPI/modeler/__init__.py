"""This package defines the Parametric Geometry API of the Modeler.
"""

# General purpose functions and abstract root classes

from services  import *
from roots     import *

# Built-in features

from part      import Part         as addPart
from sketcher  import Sketch       as addSketch
from extrusion import Extrusion    as addExtrusion
from boolean   import Addition     as addAddition
from boolean   import Subtraction  as addSubtraction
from boolean   import Intersection as addIntersection