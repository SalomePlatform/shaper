"""This package defines the Parametric Geometry API of the Modeler.
"""

# General purpose functions and abstract root classes

from services  import *
from roots     import *

# Built-in features

from part      import Part            as addPart
from sketcher.sketch  import addSketch
from construction import *
from features import *

# Custom exceptions

from errors import WrongNumberOfArguments
from errors import FeatureInputInvalid
