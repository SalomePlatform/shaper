"""Extrusion Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from ModelAPI import *


class Extrusion():

  def __init__ (self, part, sketch, size):
    """Inserts an extrusion of the given Sketch to the given Part and executes the operation."""
    self.my = part.addFeature("Extrusion")
    self.my.data().selectionList("base").append(sketch.result(), sketch.buildShape())
    if size < 0:
	  self.my.data().boolean("reverse").setValue(True)
	  size = -size
    else:
	  self.my.data().boolean("reverse").setValue(False)

    self.my.data().real("size").setValue(size)

    if ModelAPI_Session.get().validators().validate(self.my):
      self.my.execute()
    else:
      raise Exception("cannot make the Extrusion")


  def setSize (self, size):
    """Modifies the size of this extrusion according to the given size."""
    if size < 0:
	  self.my.data().boolean("reverse").setValue(True)
	  size = -size
    else:
      self.my.data().boolean("reverse").setValue(False)

    self.my.data().real("size").setValue(size)
    self.my.execute()

  def result (self):
    """Returns the result data of this Feature."""
    return self.my.firstResult()
