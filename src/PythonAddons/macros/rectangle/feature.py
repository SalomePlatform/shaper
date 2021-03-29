# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Macro-feature to produce rectangle in the sketcher
Author: Artem ZHIDKOV
"""

from salome.shaper import model
import ModelAPI
import GeomDataAPI

class SketchPlugin_Rectangle(model.Feature):
    """
    Implementation of rectangle creation.

    It produced 2 horizontal lines and 2 vertical lines connected by coincidence constraints
    """

# Initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Rectangle feature kind."""
        return "SketchRectangle"

    @staticmethod
    def START_ID():
        """Returns ID of first corner."""
        return "RectStartPoint"

    @staticmethod
    def END_ID():
        """Returns ID of second corner."""
        return "RectEndPoint"

    @staticmethod
    def AUXILIARY_ID():
        """Returns whether the rectangle is accessory."""
        return "Auxiliary"

    @staticmethod
    def LINES_LIST_ID():
        """Returns ID of list containing lines created."""
        return "RectangleList"

    @staticmethod
    def RECTANGLE_TYPE_ID():
        """Returns ID of type of rectangle creation (by corners or by center and corner)."""
        return "RectangleType"

    @staticmethod
    def RECTANGLE_BY_CORNERS_ID():
        """Returns ID of creation type by opposite corners."""
        return "RectangleTypeByCorners"

    @staticmethod
    def RECTANGLE_CENTERED_ID():
        """Returns ID of creation type by center point and a corner."""
        return "RectangleTypeCentered"

    @staticmethod
    def CENTER_ID():
        """Returns ID of center point."""
        return "RectCenterPoint"

    @staticmethod
    def CENTER_REF_ID():
        """Returns ID of the reference to the center point."""
        return "RectCenterPointRef"

    @staticmethod
    def CORNER_ID():
        """Returns ID of a corner."""
        return "RectCornerPoint"


    def getKind(self):
        """Override Feature.getKind()"""
        return SketchPlugin_Rectangle.ID()


# Initialization of the rectangle

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Flag whether the rectangle is accessory
        self.data().addAttribute(self.AUXILIARY_ID(), ModelAPI.ModelAPI_AttributeBoolean_typeId())
        # Creating corners of the rectangle
        self.data().addAttribute(self.START_ID(), GeomDataAPI.GeomDataAPI_Point2D_typeId())
        self.data().addAttribute(self.END_ID(), GeomDataAPI.GeomDataAPI_Point2D_typeId())
        # Creating list to store lines
        self.data().addAttribute(self.LINES_LIST_ID(), ModelAPI.ModelAPI_AttributeRefList_typeId())
        ModelAPI.ModelAPI_Session.get().validators().registerNotObligatory(self.getKind(), self.LINES_LIST_ID())
        # Type of rectangle
        self.data().addAttribute(self.RECTANGLE_TYPE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())
        # Center and corner of the rectangle
        self.data().addAttribute(self.CENTER_ID(), GeomDataAPI.GeomDataAPI_Point2D_typeId())
        self.data().addAttribute(self.CORNER_ID(), GeomDataAPI.GeomDataAPI_Point2D_typeId())

        self.data().addAttribute(self.CENTER_REF_ID(), ModelAPI.ModelAPI_AttributeRefAttr_typeId())
        ModelAPI.ModelAPI_Session.get().validators().registerNotObligatory(self.getKind(), self.CENTER_REF_ID())

    def isMacro(self):
        """
        Override Feature.isMacro().
        Rectangle feature is macro: removes itself on the creation transaction finish.
        """
        return True

# Edition of the rectangle

    def execute(self):
        # Retrieving list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())
        aNbLines = aLinesList.size()
        if aNbLines == 1:
            # Create 1-4 lines to compose the rectangle
            for i in range (0, 3):
                aLine = self.__sketch.addFeature("SketchLine")
                aLinesList.append(aLine)
            self.updateLines()
            aNbLines = aLinesList.size()
            aStartPoints = []
            # Create constraints to keep the rectangle
            for i in range (0, aNbLines):
                aLine = ModelAPI.objectToFeature(aLinesList.object(i))
                # connect neighbor lines by coincidence
                iPrev = i - 1
                if iPrev < 0:
                    iPrev = aNbLines - 1
                aPrevLine = ModelAPI.objectToFeature(aLinesList.object(iPrev))
                aCoincidence = self.__sketch.addFeature("SketchConstraintCoincidence")
                aRefAttrA = aCoincidence.refattr("ConstraintEntityA")
                aRefAttrB = aCoincidence.refattr("ConstraintEntityB")
                aRefAttrA.setAttr(aPrevLine.attribute("EndPoint"))
                aRefAttrB.setAttr(aLine.attribute("StartPoint"))
                aStartPoints.append(aLine.attribute("StartPoint"))
            # Flags which show horizontal or vertical constraint is build for correponding line
            self.__isHV = [False, False, False, False]
            # Update coordinates of created lines
            self.updateLines()
            # Create auxiliary diagonals in case of centered rectangle
            if self.string(self.RECTANGLE_TYPE_ID()).value() == self.RECTANGLE_CENTERED_ID():
                aDiag1 = self.__sketch.addFeature("SketchLine")
                aLinesList.append(aDiag1)
                aDiag2 = self.__sketch.addFeature("SketchLine")
                aLinesList.append(aDiag2)
                # coincidences in corners
                aPoints = [aDiag1.attribute("StartPoint"), aDiag2.attribute("StartPoint"),
                           aDiag1.attribute("EndPoint"), aDiag2.attribute("EndPoint")]
                for i in range (0, len(aPoints)):
                    aCoincidence = self.__sketch.addFeature("SketchConstraintCoincidence")
                    aRefAttrA = aCoincidence.refattr("ConstraintEntityA")
                    aRefAttrB = aCoincidence.refattr("ConstraintEntityB")
                    aRefAttrA.setAttr(aStartPoints[i])
                    aRefAttrB.setAttr(aPoints[i])
                # Update coordinates of created lines
                self.updateLines()
                aDiag1.execute()
                aDiag2.execute()
                # coincidences between center point and diagonals
                refPnt = self.getReferencePoint(self.refattr(self.CENTER_REF_ID()))
                if refPnt is not None:
                    for line in [aDiag1.lastResult(), aDiag2.lastResult()]:
                        aCoincidence = self.__sketch.addFeature("SketchConstraintCoincidence")
                        aCoincidence.refattr("ConstraintEntityA").setAttr(refPnt)
                        aCoincidence.refattr("ConstraintEntityB").setObject(line)
        # Add horizontal and vertical constraint for the lines which already have result
        for i in range (0, 4):
            if self.__isHV[i]:
                continue
            aLine = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineResult = aLine.lastResult()
            if aLineResult is None:
                continue
            aHVName = "SketchConstraintHorizontal"
            if i % 2 == 1:
                aHVName = "SketchConstraintVertical"
            aHVConstraint = self.__sketch.addFeature(aHVName)
            aRefAttrA = aHVConstraint.refattr("ConstraintEntityA")
            aRefAttrA.setObject(aLine.lastResult())
            self.__isHV[i] = True

    def attributeChanged(self, theID):
        if theID == self.START_ID() or theID == self.END_ID() or theID == self.CENTER_ID() or theID == self.CENTER_REF_ID() or theID == self.CORNER_ID():
            # Search the sketch containing this rectangle
            self.__sketch = None
            aRefs = self.data().refsToMe();
            for iter in aRefs:
                aFeature = ModelAPI.objectToFeature(iter.owner())
                if aFeature.getKind() == "Sketch":
                    self.__sketch = ModelAPI.featureToCompositeFeature(aFeature)
                    break

            aLinesList = self.reflist(self.LINES_LIST_ID())
            aNbLines = aLinesList.size()
            if aNbLines == 0:
                # Create first line to be able to create a coincidence with selected point/feature
                for i in range (0, 1):
                    aLine = self.__sketch.addFeature("SketchLine")
                    aLinesList.append(aLine)

            aStartPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.START_ID()))
            aEndPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.END_ID()))
            aCenter = self.getPointByRef(self.attribute(self.CENTER_ID()), self.refattr(self.CENTER_REF_ID()))
            aCorner = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.CORNER_ID()))
            if (aStartPoint.isInitialized() and aEndPoint.isInitialized()) or (aCenter is not None and aCorner.isInitialized()):
              self.updateLines()
            else:
              self.updateStartPoint()
        if theID == self.AUXILIARY_ID():
            anAuxiliary = self.data().boolean(self.AUXILIARY_ID()).value()
            aLinesList = self.reflist(self.LINES_LIST_ID())
            aNbLines = aLinesList.size()
            # Update coordinates of rectangle lines
            for i in range (0, aNbLines):
                aLine = ModelAPI.objectToFeature(aLinesList.object(i))
                aLine.data().boolean("Auxiliary").setValue(anAuxiliary)

    def getReferencePoint(self, theRef):
        if theRef.isObject() and theRef.object() is not None:
            feature = ModelAPI.ModelAPI_Feature.feature(theRef.object())
            if feature.getKind() == "SketchPoint":
                return feature.attribute("PointCoordinates")
        else:
            return theRef.attr()
        return None

    def getPointByRef(self, thePoint, theRef):
        attr = thePoint
        if theRef.isInitialized():
            refPnt = self.getReferencePoint(theRef)
            if refPnt is not None:
                attr = refPnt
        if attr is None or not attr.isInitialized():
            return None
        return GeomDataAPI.geomDataAPI_Point2D(attr).pnt()

    def updateLines(self):
        # Retrieving list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())
        aNbLines = min(aLinesList.size(), 4)
        if self.string(self.RECTANGLE_TYPE_ID()).value() == self.RECTANGLE_CENTERED_ID():
            aCenter = self.getPointByRef(self.attribute(self.CENTER_ID()), self.refattr(self.CENTER_REF_ID()))
            aCorner = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.CORNER_ID()))
            aStartX = 2.0 * aCenter.x() - aCorner.x()
            aStartY = 2.0 * aCenter.y() - aCorner.y()
            aX = [aStartX, aStartX, aCorner.x(), aCorner.x()]
            aY = [aStartY, aCorner.y(), aCorner.y(), aStartY]
        else:
            aStartPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.START_ID()))
            aEndPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.END_ID()))
            aX = [aStartPoint.x(), aStartPoint.x(), aEndPoint.x(), aEndPoint.x()]
            aY = [aStartPoint.y(), aEndPoint.y(), aEndPoint.y(), aStartPoint.y()]
        anAuxiliary = self.data().boolean(self.AUXILIARY_ID()).value()

        # do not recalculate the rectrangle after each update
        wasBlocked = []
        for i in range (0, aLinesList.size()):
            wasBlocked.append(aLinesList.object(i).data().blockSendAttributeUpdated(True))

        # Update coordinates of rectangle lines
        for i in range (0, aNbLines):
            aLine = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("StartPoint"))
            aLineEnd = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
            aLineStart.setValue(aX[i-1], aY[i-1])
            aLineEnd.setValue(aX[i], aY[i])
            aLine.data().boolean("Auxiliary").setValue(anAuxiliary)

        # Update auxiliary diagonals
        if self.string(self.RECTANGLE_TYPE_ID()).value() == self.RECTANGLE_CENTERED_ID():
            for i in range (aNbLines, aLinesList.size()):
                aLine = ModelAPI.objectToFeature(aLinesList.object(i))
                aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("StartPoint"))
                aLineEnd = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
                aLineStart.setValue(aX[i-aNbLines-1], aY[i-aNbLines-1])
                aLineEnd.setValue(aX[i-aNbLines+1], aY[i-aNbLines+1])
                aLine.data().boolean("Auxiliary").setValue(True)

        # update the rectangle
        for i in range (0, aLinesList.size()):
            aLinesList.object(i).data().blockSendAttributeUpdated(wasBlocked[i], True)

    def updateStartPoint(self):
        # Retrieving list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())
        aNbLines = aLinesList.size()

        aStartPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.START_ID()))
        if aStartPoint.isInitialized:
            aX = aStartPoint.x()
            aY = aStartPoint.y()
        else:
            aCenter = self.getPointByRef(self.attribute(self.CENTER_ID()), self.refattr(self.CENTER_REF_ID()))
            aX = aCenter.x()
            aY = aCenter.y()

        # Update coordinates of rectangle lines
        for i in range (0, aNbLines):
            aLine = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
            aLineStart.setValue(aX, aY)
