"""Abstract root classes of user-defined Python features producing a Body
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from ModelAPI import *


class Feature(ModelAPI_Feature):
  """Base class of user-defined Python features."""

  def __init__(self):
    """Constructor"""
    ModelAPI_Feature.__init__(self)

  def addRealInput (self, inputid):
    """Add double input"""
    self.data().addAttribute(inputid, ModelAPI_AttributeDouble_typeId())

  def getRealInput (self, inputid):
    """Returns double input"""
    return self.data().real(inputid).value()

  def addResult (self, result):
    """Add result"""
    shape = result.shape()
    body  = self.document().createBody( self.data() )
    body.store(shape)
    self.setResult(body)


class Interface():
  """Base class of hight level Python interfaces to features."""

  def __init__(self, container, fid):
    """Constructor"""
    ### Create the feature
    self.my = container.addFeature(fid)

  def setRealInput (self, inputid, value):
    """Set real value"""
    self.my.data().real(inputid).setValue(value)

  def areInputValid (self):
    """Returns True if the input is valid"""
    return ModelAPI_Session.get().validators().validate(self.my)

  def execute (self):
    """Build the feature result"""
    self.my.execute()