"""Package for Features plugin for the Parametric Geometry API of the Modeler.
"""

from boolean   import Addition        as addAddition
from boolean   import Subtraction     as addSubtraction
from boolean   import Intersection    as addIntersection

from partition import addPartition

from extrusion import addExtrusion
from extrusion_boolean import addExtrusionCut, addExtrusionFuse
from extrusion_sketch import addExtrusionSketch

from revolution import addRevolution
from revolution_boolean import addRevolutionCut, addRevolutionFuse
from revolution_sketch import addRevolutionSketch
