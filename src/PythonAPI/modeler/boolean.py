"""Boolean operations Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from ModelAPI    import *
from GeomAlgoAPI import *


class Boolean():
  """Abstract root class of Boolean Features."""
  def __init__(self, part, object, tool, type):
    self.my = part.addFeature("Boolean")
    self.my.data().reference("main_object").setValue(object)
    self.my.data().reference("tool_object").setValue(tool)
    self.my.data().integer("bool_type").setValue(type)

    if ModelAPI_Session.get().validators().validate(self.my):
      self.my.execute()
    else:
      raise Exception("cannot make the Boolean")


class Addition(Boolean):

  def __init__(self, part, object, tool):
    """Inserts an addition to the given Part and executes the operation.
    This operation adds tool to the given object.
    """
    Boolean.__init__(self, part, object, tool, GeomAlgoAPI_Boolean.BOOL_FUSE)


class Subtraction(Boolean):

  def __init__(self, part, object, tool):
    """Inserts a subtraction to the given Part and executes the operation.
    This operation subtracts tool to the given object.
    """
    Boolean.__init__(self, part, object, tool, GeomAlgoAPI_Boolean.BOOL_CUT)


class Intersection(Boolean):

  def __init__(self, part, object, tool):
    """Inserts an intersection to the given Part and executes the operation.
    This operation intersects tool to the given object.
    """
    Boolean.__init__(self, part, object, tool, GeomAlgoAPI_Boolean.BOOL_COMMON)