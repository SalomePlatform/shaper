"""Package for Features plugin for the Parametric Geometry API of the Modeler.
"""

from FeaturesAPI import addFuse, addCut, addCommon

from partition import addPartition

from FeaturesAPI import addExtrusion, addRevolution
from extrusion_boolean import addExtrusionCut, addExtrusionFuse
from revolution_boolean import addRevolutionCut, addRevolutionFuse

from placement import addPlacement
from rotation import addRotation
from translation import addTranslation

from group import addGroup
