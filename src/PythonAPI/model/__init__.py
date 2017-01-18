"""This package defines the Parametric Geometry API of the Modeler.

All features are available via model.add*() functions. Exceptions are:
- exportToFile() - from Exchange plugin
- duplicatePart(), removePart() - from PartSet plugin
- exportToGEOM() - from Connection plugin
"""

# General purpose functions and abstract root classes

from services  import *
from roots     import *

# Built-in features

from build import *
from sketcher import *
from connection import *
from construction import *
from exchange import *
from features import *
from collection import *
from parameter import *
from partset import *
from primitives import *
from gdml import *
from tests import *
