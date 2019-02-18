# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
Box macro-feature
Authors: Renaud Nedelec - Daniel Brunier-Coulin
"""

from salome.shaper import model
from salome.shaper import geom


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
        return "Box_script"

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

        line = model.addPolygon(self.base, p1, p2, p3, p4)

        self.base.setFixed(line[0].startPoint())
        self.base.setVertical(line[0])

        # Setting the size of the base with default values
        # Width
        self.width = self.base.setLength(line[3], 50)  # Keeps the constraint for edition
        # Length
        self.length = self.base.setLength(line[0], 50)  # Keeps the constraint for edition

        # Keeping the rectangle
        self.base.setParallel(line[0], line[2])
        self.base.setParallel(line[1], line[3])
        self.base.setPerpendicular(line[0], line[3])

        # execute sketch
        mypart.setCurrentFeature(self.base.feature(), False)
        model.updateFeatures()

        # Creating the extrusion (the box) at default size
        # A box result
        self.box = model.addExtrusion(mypart, self.base.selectFace(), 50)

# Edition of the box at user size

    def execute(self):
        """F.execute() -- execute the feature"""
        # Retrieving the user inputs
        width = self.real(self.WIDTH_ID())
        length = self.real(self.LENGTH_ID())
        height = self.real(self.HEIGHT_ID())

        # Editing the box
        if width.text() == "":
            self.base.setValue(self.width, width.value())
        else:
            self.base.setValue(self.width, width.text())

        if length.text() == "":
            self.base.setValue(self.length, length.value())
        else:
            self.base.setValue(self.length, length.text())

        if (height.text() == ""):
            self.box.setSize(height.value())
        else:
            self.box.setSize(height.text())

        # Publishing the result: not needed for Macro feature
        # self.addResult( self.box.result() )

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        Box feature is macro: removes itself on the creation transaction
        finish.
        """
        return True
