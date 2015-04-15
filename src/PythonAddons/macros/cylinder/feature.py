"""Cylinder macro-feature
Authors: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import modeler
import geom


class CylinderFeature(modeler.Feature):


# Initializations

  def __init__(self):
    modeler.Feature.__init__(self)

  @staticmethod
  def ID():
    return "Cylinder"

  @staticmethod
  def RADIUS_ID():
    return "radius"

  @staticmethod
  def LENGTH_ID():
    return "length"

  def getKind(self):
    return CylinderFeature.ID()

	
# Creation of the cylinder at default size

  def initAttributes(self):

    # Creating the input arguments of the feature
    self.addRealInput( self.RADIUS_ID() )
    self.addRealInput( self.LENGTH_ID() )

    # Creating the base of the cylinder with unit values
    mypart = modeler.activeDocument()
    xoy    = modeler.defaultPlane("XOY")

    self.base = modeler.addSketch( mypart, xoy )
    circle    = self.base.addCircle( 0, 0, 1)

    # Setting the radius of the base with default values
    self.radius = self.base.setRadius( circle.result(), 10 )    # Keeps the constraint for edition

    # Creating the extrusion (the cylinder) at default size
    self.cyl = modeler.addExtrusion( mypart, self.base.selectFace(), 50 )

	
# Edition of the cylinder at user size

  def execute(self):
    # Retrieving the user inputs
    radius = self.getRealInput( self.RADIUS_ID() )
    length = self.getRealInput( self.LENGTH_ID() )

    # Editing the cylinder
    self.base.setValue( self.radius, radius )
    self.cyl.setSize( length )

	# Publishing the result
    self.addResult( self.cyl.result() )