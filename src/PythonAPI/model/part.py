"""Part Feature Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import model     # Required by the temporary implementation of result member function


class Part():

  def __init__ (self, partset):
    """Adds a new Part to the given Partset and activates the Part."""
    self._feature = partset.addFeature("Part")
    self._feature.execute()

  def document (self):
    """Returns the Part document created by this feature."""
    #TODO: Get the document referenced by this feature
    return model.activeDocument()