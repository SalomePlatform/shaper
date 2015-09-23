"""Box macro-feature Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import modeler
from macros.box.feature import BoxFeature as MY


class Box(modeler.Interface):
  """Executes the macro-feature Box.
  """
  def __init__(self, part, dx, dy, dz):
    """Constructor"""
    modeler.Interface.__init__(self, part, MY.ID())

    self.setRealInput( MY.WIDTH_ID(), dx )
    self.setRealInput( MY.LENGTH_ID(), dy )
    self.setRealInput( MY.HEIGHT_ID(), dz )

    if self.areInputValid():
      self.execute()
    else:
      raise Exception("cannot make the Box")