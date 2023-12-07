# Copyright (C) 2014-2024  CEA, EDF
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
        self.data().addAttribute(self.AUXILIARY_ID(), ModelAPI.ModelAPI_AttributeBoolean.typeId())
        # Corners of the rectangle (being defined by opposite corners)
        self.data().addAttribute(self.START_ID(), GeomDataAPI.GeomDataAPI_Point2D.typeId())
        self.data().addAttribute(self.END_ID(), GeomDataAPI.GeomDataAPI_Point2D.typeId())
        # List with both contour and diagonal lines
        self.data().addAttribute(self.LINES_LIST_ID(), ModelAPI.ModelAPI_AttributeRefList.typeId())
        ModelAPI.ModelAPI_Session.get().validators().registerNotObligatory(self.getKind(), self.LINES_LIST_ID())
        # Type of rectangle
        self.data().addAttribute(self.RECTANGLE_TYPE_ID(), ModelAPI.ModelAPI_AttributeString.typeId())
        # Center and corner of centered rectangle (being defined by center and corner)
        self.data().addAttribute(self.CENTER_ID(), GeomDataAPI.GeomDataAPI_Point2D.typeId())
        self.data().addAttribute(self.CORNER_ID(), GeomDataAPI.GeomDataAPI_Point2D.typeId())

        self.data().addAttribute(self.CENTER_REF_ID(), ModelAPI.ModelAPI_AttributeRefAttr.typeId())
        ModelAPI.ModelAPI_Session.get().validators().registerNotObligatory(self.getKind(), self.CENTER_REF_ID())

    def isMacro(self):
        """
        Override Feature.isMacro().
        Rectangle feature is macro: removes itself on the creation transaction finish.
        """
        return True


# Editing of a rectangle. It is called on select of the first generative point
# and on hover of the second generative point. Generative point is either corner or center,
# depending on rectangle type. And also it is called on call of Sketch.addRectangleCentered(...) in TUI.

    def execute(self):
        # Retrieve list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())        
        if aLinesList.size() == 1:
            # Create remaining rectangle contour lines
            for i in range (1, 4):
                aLine = self.__sketch.addFeature("SketchLine")
                aLinesList.append(aLine)
            self.__updateLines()

            # Connect rectangle contour lines
            aStartPoints = []
            for i in range (0, 4):
                aLine = ModelAPI.objectToFeature(aLinesList.object(i))
                iPrev = i - 1 if i != 0 else 3
                aPrevLine = ModelAPI.objectToFeature(aLinesList.object(iPrev))
                aCoincidence = self.__sketch.addFeature("SketchConstraintCoincidence")
                aCoincidence.refattr("ConstraintEntityA").setAttr(aPrevLine.attribute("EndPoint"))
                aCoincidence.refattr("ConstraintEntityB").setAttr(aLine.attribute("StartPoint"))
                aStartPoints.append(aLine.attribute("StartPoint"))

            # Update coordinates of created lines
            self.__updateLines()

            # Flags, indicating perpendicular constraint is imposed on contour lines i and i+1.
            self.__isPERP = [False, False, False]

            # Create auxiliary diagonals in case of centered rectangle
            if self.string(self.RECTANGLE_TYPE_ID()).value() == self.RECTANGLE_CENTERED_ID():
                aDiag1 = self.__sketch.addFeature("SketchLine")
                aLinesList.append(aDiag1)
                aDiag2 = self.__sketch.addFeature("SketchLine")
                aLinesList.append(aDiag2)

                # Add coincidences between diagonals' endpoints and rectangle vertices
                aPoints = [aDiag1.attribute("StartPoint"), aDiag2.attribute("StartPoint"),
                           aDiag1.attribute("EndPoint"), aDiag2.attribute("EndPoint")]
                
                for i in range (0, len(aPoints)):
                    aCoincidence = self.__sketch.addFeature("SketchConstraintCoincidence")
                    aCoincidence.refattr("ConstraintEntityA").setAttr(aStartPoints[i])
                    aCoincidence.refattr("ConstraintEntityB").setAttr(aPoints[i])

                # Update coordinates of created lines
                self.__updateLines()
                aDiag1.execute()
                aDiag2.execute()

                # coincidences between center point and diagonals
                attr = self.__getPoint2DAttrOfSketchPoint(self.refattr(self.CENTER_REF_ID()))
                if attr is not None:
                    for line in [aDiag1.lastResult(), aDiag2.lastResult()]:
                        aCoincidence = self.__sketch.addFeature("SketchConstraintCoincidence")
                        aCoincidence.refattr("ConstraintEntityA").setAttr(attr)
                        aCoincidence.refattr("ConstraintEntityB").setObject(line)

        # Add perpendicular constraints to the contour lines, which already have result
        for i in range (0, 3):
            if self.__isPERP[i]:
                continue
            
            aLine_A = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineResult_A = aLine_A.lastResult()
            if aLineResult_A is None:
                continue

            aLine_B = ModelAPI.objectToFeature(aLinesList.object(i+1))
            aLineResult_B = aLine_B.lastResult()
            if aLineResult_B is None:
                continue

            aConstraintPerp = self.__sketch.addFeature("SketchConstraintPerpendicular")
            aConstraintPerp.refattr("ConstraintEntityA").setObject(aLine_A.lastResult())
            aConstraintPerp.refattr("ConstraintEntityB").setObject(aLine_B.lastResult())
            self.__isPERP[i] = True


    def attributeChanged(self, theID):
        if theID == self.START_ID() or theID == self.END_ID() or theID == self.CENTER_ID() or theID == self.CENTER_REF_ID() or theID == self.CORNER_ID():
            # Find the sketch containing this rectangle
            self.__sketch = None
            aRefsToMe = self.data().refsToMe()
            for aRefToMe in aRefsToMe:
                aFeature = ModelAPI.objectToFeature(aRefToMe.owner())
                if aFeature.getKind() == "Sketch":
                    self.__sketch = ModelAPI.featureToCompositeFeature(aFeature)
                    break

            if theID == self.CENTER_ID():
                aCenter = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.CENTER_ID())) # shared_ptr to Point2D
                aCenterSketchPointAttr = self.refattr(self.CENTER_REF_ID())
                if (not aCenterSketchPointAttr.isInitialized()):
                    # Create SketchPoint. In .execute() it will be constrained to keep center.
                    aCenterSketchPoint = self.__sketch.addFeature("SketchPoint")
                    aCenterSketchPoint.data().boolean("Auxiliary").setValue(True)
                    aCenterSketchPointCoords = GeomDataAPI.geomDataAPI_Point2D(aCenterSketchPoint.attribute("PointCoordinates")) # shared_ptr to Point2D                    
                    aCenterSketchPointCoords.setValue(aCenter.x(), aCenter.y())
                    aCenterSketchPointAttr.setObject(aCenterSketchPoint)
                else:
                    # Maintain consistency between center SketchPoint and center Point2D.
                    aCenterSketchPointCoordsAttr = self.__getPoint2DAttrOfSketchPoint(aCenterSketchPointAttr)
                    if (aCenterSketchPointCoordsAttr == None):
                        Warning("Faulty logic")
                    else:
                        aCenterSketchPointCoords = GeomDataAPI.geomDataAPI_Point2D(aCenterSketchPointCoordsAttr) # shared_ptr to Point2D
                        aCenterSketchPointCoords.setValue(aCenter.x(), aCenter.y())
            elif theID == self.CENTER_REF_ID():
                aCenterSketchPointAttr = self.refattr(self.CENTER_REF_ID())
                aCenterSketchPointCoordsAttr = self.__getPoint2DAttrOfSketchPoint(aCenterSketchPointAttr) # shared_ptr to Point2D
                if (aCenterSketchPointCoordsAttr == None):
                    Warning("Faulty logic. Attempt to set rectangle's attribute " + self.CENTER_REF_ID() + " not with refattr to SketchPoint.")
                    return
                
                # Maintain consistency between center SketchPoint and center Point2D.
                aCenterSketchPointCoords = GeomDataAPI.geomDataAPI_Point2D(aCenterSketchPointCoordsAttr) # shared_ptr to Point2D
                aCenter = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.CENTER_ID())) # shared_ptr to Point2D
                aCenter.setValue(aCenterSketchPointCoords.x(), aCenterSketchPointCoords.y())

            aLinesList = self.reflist(self.LINES_LIST_ID())
            aNbLines = aLinesList.size()
            if aNbLines == 0:
                # If only one generative point is iniialized,
                # do not create the full set of contour lines to not clutter
                # UI with icons of constraints near the mouse pointer.
                aLine = self.__sketch.addFeature("SketchLine")
                aLinesList.append(aLine)                    

            aStartPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.START_ID()))
            aEndPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.END_ID()))
            aCenter = self.__getPoint2D(self.attribute(self.CENTER_ID()), self.refattr(self.CENTER_REF_ID()))
            aCorner = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.CORNER_ID()))

            if (aStartPoint.isInitialized() and aEndPoint.isInitialized()) or (aCenter is not None and aCorner.isInitialized()):
              self.__updateLines()
            else:
              self.__updateLinesWithOnlyGenerativePoint()
        elif theID == self.AUXILIARY_ID():
            # Change aux attribute of contour lines
            anAuxiliary = self.data().boolean(self.AUXILIARY_ID()).value()
            aLinesList = self.reflist(self.LINES_LIST_ID())            
            for i in range (0, min(aLinesList.size(), 4)):
                aLine = ModelAPI.objectToFeature(aLinesList.object(i))
                aLine.data().boolean("Auxiliary").setValue(anAuxiliary)
        elif theID == self.RECTANGLE_TYPE_ID():
            # TODO Find a way to distinguish "attribute changed" events on hover and on click.
            # Now, if both generative points are selected, but the rectangle is not applied,
            # and then rectangle type is changed, the unapplied rectangle is erased. 
            # It should be applied instead.
            aLinesList = self.reflist(self.LINES_LIST_ID()).clear()
            aCenterSketchPointAttr = self.refattr(self.CENTER_REF_ID())
            aCenterSketchPointAttr.reset()
            self.attribute(self.START_ID()).reset()
            self.attribute(self.END_ID()).reset()
            self.attribute(self.CENTER_ID()).reset()
            self.attribute(self.CORNER_ID()).reset()


    def __getPoint2DAttrOfSketchPoint(self, theSketchPointRefAttr):
        if theSketchPointRefAttr.isObject() and theSketchPointRefAttr.object() is not None:
            feature = ModelAPI.ModelAPI_Feature.feature(theSketchPointRefAttr.object())
            if feature.getKind() == "SketchPoint":
                return feature.attribute("PointCoordinates")
        else:
            return theSketchPointRefAttr.attr()


    def __getPoint2D(self, thePoint2DAttr, theSketchPointRefAttr):
        attr = thePoint2DAttr
        if theSketchPointRefAttr.isInitialized():
            aPoint2DAttr = self.__getPoint2DAttrOfSketchPoint(theSketchPointRefAttr)
            if aPoint2DAttr is not None:
                attr = aPoint2DAttr
        if attr is None or not attr.isInitialized():
            return None
        return GeomDataAPI.geomDataAPI_Point2D(attr).pnt()


    def __updateLines(self):
        if self.string(self.RECTANGLE_TYPE_ID()).value() == self.RECTANGLE_CENTERED_ID():
            aCenter = self.__getPoint2D(self.attribute(self.CENTER_ID()), self.refattr(self.CENTER_REF_ID()))
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

        # Retrieve list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())
        aNumOfContourLines = min(aLinesList.size(), 4)

        # Do not update lines during update of coordinates
        wasBlocked = []
        for i in range (0, aLinesList.size()):
            wasBlocked.append(aLinesList.object(i).data().blockSendAttributeUpdated(True))

        # Update coordinates of rectangle lines
        anAuxiliary = self.data().boolean(self.AUXILIARY_ID()).value()
        for i in range (0, aNumOfContourLines):
            aLine = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("StartPoint"))
            aLineEnd = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
            aLineStart.setValue(aX[i-1], aY[i-1])
            aLineEnd.setValue(aX[i], aY[i])
            aLine.data().boolean("Auxiliary").setValue(anAuxiliary)

        # Update auxiliary diagonals
        if self.string(self.RECTANGLE_TYPE_ID()).value() == self.RECTANGLE_CENTERED_ID():
            for i in range (aNumOfContourLines, aLinesList.size()):
                aLine = ModelAPI.objectToFeature(aLinesList.object(i))
                aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("StartPoint"))
                aLineEnd = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
                aLineStart.setValue(aX[i-aNumOfContourLines-1], aY[i-aNumOfContourLines-1])
                aLineEnd.setValue(aX[i-aNumOfContourLines+1], aY[i-aNumOfContourLines+1])
                aLine.data().boolean("Auxiliary").setValue(True)

        # Update the rectangle
        for i in range (0, aLinesList.size()):
            aLinesList.object(i).data().blockSendAttributeUpdated(wasBlocked[i], True)


    def __updateLinesWithOnlyGenerativePoint(self):
        # Retrieve list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())
        aNbLines = aLinesList.size()

        aStartPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.START_ID()))
        if aStartPoint.isInitialized:
            aX = aStartPoint.x()
            aY = aStartPoint.y()
        else:
            aCenter = self.__getPoint2D(self.attribute(self.CENTER_ID()), self.refattr(self.CENTER_REF_ID()))
            aX = aCenter.x()
            aY = aCenter.y()

        # Update coordinates of rectangle lines
        for i in range (0, aNbLines):
            aLine = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
            aLineStart.setValue(aX, aY)
