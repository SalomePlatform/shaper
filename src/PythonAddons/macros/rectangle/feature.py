"""
Macro-feature to produce rectangle in the sketcher
Author: Artem ZHIDKOV
Copyright (C) 2016-20xx CEA/DEN, EDF R&D
"""

import model
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
            aNbLines = aLinesList.size()
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
            # Flags which show horizontal or vertical constraint is build for correponding line
            self.__isHV = [False, False, False, False]
            # Update coordinates of created lines
            self.updateLines()
        # Add horizontal and vertical constraint for the lines which already have result
        for i in range (0, aNbLines):
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
        if theID == self.START_ID() or theID == self.END_ID():
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
            if aStartPoint.isInitialized() and aEndPoint.isInitialized():
              self.updateLines()
            else:
              self.updateStartPoint()

    def updateLines(self):
        # Retrieving list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())
        aNbLines = aLinesList.size()
        aStartPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.START_ID()))
        aEndPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.END_ID()))
        aX = [aStartPoint.x(), aStartPoint.x(), aEndPoint.x(), aEndPoint.x()]
        aY = [aStartPoint.y(), aEndPoint.y(), aEndPoint.y(), aStartPoint.y()]

        # Update coordinates of rectangle lines
        for i in range (0, aNbLines):
            aLine = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("StartPoint"))
            aLineEnd = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
            aLineStart.setValue(aX[i-1], aY[i-1])
            aLineEnd.setValue(aX[i], aY[i])

    def updateStartPoint(self):
        # Retrieving list of already created lines
        aLinesList = self.reflist(self.LINES_LIST_ID())
        aNbLines = aLinesList.size()

        aStartPoint = GeomDataAPI.geomDataAPI_Point2D(self.attribute(self.START_ID()))
        aX = aStartPoint.x()
        aY = aStartPoint.y()

        # Update coordinates of rectangle lines
        for i in range (0, aNbLines):
            aLine = ModelAPI.objectToFeature(aLinesList.object(i))
            aLineStart = GeomDataAPI.geomDataAPI_Point2D(aLine.attribute("EndPoint"))
            aLineStart.setValue(aX, aY)
