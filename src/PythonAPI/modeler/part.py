"""Part Feature Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import modeler     # Required by the temporary implementation of result member function


class Part():
  """Class for Part feature"""
  
  def __init__ (self, partset):
    """Adds a new Part to the given Partset and activates the Part."""
    ### Create the feature
    self.my = partset.addFeature("Part")
    self.my.execute()

  def document (self):
    """Returns the Part document created by this feature."""
    #TODO: Get the document referenced by this feature
    return modeler.activeDocument()