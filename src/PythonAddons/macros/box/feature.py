"""Box macro-feature
Authors: Renaud Nedelec - Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import model
import geom


class BoxFeature(model.Feature):
    """An example of Box feature implementation.

    BoxFeature() -> Box
    """

# Initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the feature."""
        return "Box"

    @staticmethod
    def WIDTH_ID():
        """Returns ID of Width parameter."""
        return "width"

    @staticmethod
    def LENGTH_ID():
        """Returns ID of Length parameter."""
        return "length"

    @staticmethod
    def HEIGHT_ID():
        """Returns ID of Height parameter."""
        return "height"

    def getKind(self):
        """Override Feature.getKind()"""
        return BoxFeature.ID()


# Creation of the box at default size

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Creating the input arguments of the feature
        self.addRealInput(self.WIDTH_ID())
        self.addRealInput(self.LENGTH_ID())
        self.addRealInput(self.HEIGHT_ID())

        # Creating the base of the box with unit values
        mypart = model.activeDocument()
        xoy = model.defaultPlane("XOY")

        # A base of the geometry
        self.base = model.addSketch(mypart, xoy)

        p1 = geom.Pnt2d(0, 0)
        p2 = geom.Pnt2d(0, 1)
        p3 = geom.Pnt2d(1, 1)
        p4 = geom.Pnt2d(1, 0)

        line = self.base.addPolygon(p1, p2, p3, p4)

        self.base.setParallel(line[0].result(), line[2].result())
        self.base.setParallel(line[1].result(), line[3].result())
        self.base.setPerpendicular(line[0].result(), line[3].result())

        # Setting the size of the base with default values
        # Width
        self.width = self.base.setLength(line[0].result(), 50)  # Keeps the constraint for edition
        # Length
        self.length = self.base.setLength(line[3].result(), 50)  # Keeps the constraint for edition

        # Creating the extrusion (the box) at default size
        # A box result
        self.box = model.addExtrusion(mypart, self.base.selectFace(), 50)

# Edition of the box at user size

    def execute(self):
        """F.execute() -- execute the feature"""
        # Retrieving the user inputs
        width = self.getRealInput(self.WIDTH_ID())
        length = self.getRealInput(self.LENGTH_ID())
        height = self.getRealInput(self.HEIGHT_ID())

        width_text = self.getTextInput(self.WIDTH_ID())
        length_text = self.getTextInput(self.LENGTH_ID())
        height_text = self.getTextInput(self.HEIGHT_ID())

        # Editing the box
        if width_text == "":
            self.base.setValue(self.width, width)
        else:
            self.base.setText(self.width, width_text)

        if length_text == "":
            self.base.setValue(self.length, length)
        else:
            self.base.setText(self.length, length_text)

        self.box.setSize(height, height_text)

        # Publishing the result: not needed for Macro feature
        # self.addResult( self.box.result() )

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        Box feature is macro: removes itself on the creation transaction
        finish.
        """
        return True
