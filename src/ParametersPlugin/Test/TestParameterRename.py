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
    TestParameterCreation.py

    class ParametersPlugin_Parameter
    static const std::string MY_PARAMETER_ID("Parameter");
    static const std::string MY_VARIABLE_ID("variable");
    static const std::string MY_EXPRESSION_ID("expression");

    data()->addAttribute(ParametersPlugin_Parameter::VARIABLE_ID(),
                         ModelAPI_AttributeString::typeId());
    data()->addAttribute(ParametersPlugin_Parameter::EXPRESSION_ID(),
                         ModelAPI_AttributeString::typeId());

    class ModelAPI_ResultParameter
    static const std::string MY_VALUE_ID("Value");
    static const std::string MY_VALUE_ID("State");
"""

#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *
import math
import unittest
from salome.shaper import model

__updated__ = "2015-04-27"

#=========================================================================
# Create several parameters and a feature.
# 1. Basic parameter definition:
#    x1 = 150.0, y1 = 200.0
# 2. Referencing between parameters:
#    x2 = x1 + y1 + 100.0
# 3. Referencing in feature SketchCircle
#    CircleCenter = (x1 + 10.0, x1 + 20.0), CircleRadius = x1
# 3. Check renaming
#    x1 -> a1
# 4. Check renaming to not unique name
#    x1 -> y1
#=========================================================================
class TestParameterRename(unittest.TestCase):
    def setUp(self):
        self.aSession = ModelAPI_Session.get()
        model.begin()
        partSet = self.aSession.moduleDocument()
        Part = model.addPart(partSet)
        model.end()
        self.aDocument = Part.document()
        self.createParameters()
        self.createFeature()

    def tearDown(self):
        #assert(model.checkPythonDump())
        #self.aSession.closeAll()
        pass

    def createParameters(self):
        ltNames = ["x1", "y1", "x2", "n", "px"]
        ltExpressions = ["150.", "200.", "x1 + y1 + 100.0", "5", "50"]
        self.dtParams = {}
        for name, expr in zip(ltNames, ltExpressions):
            self.aSession.startOperation()
            aParam = self.aDocument.addFeature("Parameter")
            aParamName = aParam.string("variable")
            aParamName.setValue(name)
            aParamExpr = aParam.string("expression")
            aParamExpr.setValue(expr)
            self.dtParams[name] = aParam
            self.aSession.finishOperation()
        self.assertEqual(len(self.dtParams), len(ltNames))

        aParam = self.dtParams["x2"]
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.assertEqual(aResultAttr.data().real("Value").value(), 450.)

    def createFeature(self):
        self.aSession.startOperation()
        aSketchCommonFeature = self.aDocument.addFeature("Sketch")
        aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
        origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
        origin.setValue(0, 0, 0)
        dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
        dirx.setValue(1, 0, 0)
        norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
        norm.setValue(0, 0, 1)
        aSketchCircle = aSketchFeature.addFeature("SketchCircle")
        anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
        aRadiusAttr = aSketchCircle.real("circle_radius")
        anCircleCentr.setValue(10., 20.)
        aRadiusAttr.setValue(10.)
        self.aSession.finishOperation()

        self.anCircleCentr = anCircleCentr
        self.aRadiusAttr = aRadiusAttr

        # constraints to fix circle position and radius
        self.aSession.startOperation()
        # fix X coordinate
        aDistanceConstraint1 = aSketchFeature.addFeature("SketchConstraintDistance")
        refattrA = aDistanceConstraint1.refattr("ConstraintEntityA")
        refattrA.setAttr(anCircleCentr)
        refattrB = aDistanceConstraint1.refattr("ConstraintEntityB")
        anOY = aSketchFeature.addFeature("SketchLine")
        aStartPoint = geomDataAPI_Point2D(anOY.attribute("StartPoint"))
        anEndPoint = geomDataAPI_Point2D(anOY.attribute("EndPoint"))
        aStartPoint.setValue(0., 0.)
        anEndPoint.setValue(0., 100.)
        anOY.selection("External").selectSubShape("EDGE", "PartSet/OY")
        anOY.execute()
        refattrB.setObject(modelAPI_ResultConstruction(anOY.firstResult()))
        valueX = aDistanceConstraint1.real("ConstraintValue")
        valueX.setText("x1 + 10.0")
        aDistanceConstraint1.execute();
        # fix Y coordinate
        aDistanceConstraint2 = aSketchFeature.addFeature("SketchConstraintDistance")
        refattrA = aDistanceConstraint2.refattr("ConstraintEntityA")
        refattrA.setAttr(anCircleCentr)
        refattrB = aDistanceConstraint2.refattr("ConstraintEntityB")
        anOX = aSketchFeature.addFeature("SketchLine")
        aStartPoint = geomDataAPI_Point2D(anOX.attribute("StartPoint"))
        anEndPoint = geomDataAPI_Point2D(anOX.attribute("EndPoint"))
        aStartPoint.setValue(0., 0.)
        anEndPoint.setValue(100., 0.)
        anOX.selection("External").selectSubShape("EDGE", "PartSet/OX")
        anOX.execute()
        refattrB.setObject(modelAPI_ResultConstruction(anOX.firstResult()))
        valueY = aDistanceConstraint2.real("ConstraintValue")
        valueY.setText("x1 + 20.0")
        aDistanceConstraint2.execute();
        # fix radius
        aRadiusConstraint = aSketchFeature.addFeature("SketchConstraintRadius")
        refattrA = aRadiusConstraint.refattr("ConstraintEntityA")
        refattrA.setObject(modelAPI_ResultConstruction(aSketchCircle.lastResult()))
        aRadiusConstrAttr = aRadiusConstraint.real("ConstraintValue")
        aRadiusConstrAttr.setText("x1")
        aRadiusConstraint.execute()
        self.aSession.finishOperation()

        self.aCircleCenterX = valueX
        self.aCircleCenterY = valueY
        self.aCircleRadius = aRadiusConstrAttr

        self.assertEqual(self.anCircleCentr.x(), 160.)
        self.assertEqual(self.anCircleCentr.y(), 170.)
        self.assertEqual(aRadiusAttr.value(), 150.)

        # add a line and rotate it around origin
        self.aSession.startOperation()
        aSketchLine = aSketchFeature.addFeature("SketchLine")
        aStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
        aEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
        aStartPoint.setText("px", "0")
        aEndPoint.setValue(0., 0.)
        self.aSession.finishOperation()
        self.aSession.startOperation()
        aEndPoint.move(100., 0.)
        self.aSession.finishOperation()

        self.aSession.startOperation()
        aMultiRotation = aSketchFeature.addFeature("SketchMultiRotation")
        aMultiRotation.reflist("MultiRotationList").append(aSketchLine.lastResult())
        aMultiRotation.refattr("MultiRotationCenter").setAttr(anCircleCentr)
        aMultiRotation.real("MultiRotationAngle").setValue(60.0)
        aMultiRotation.string("AngleType").setValue("SingleAngle")
        aMultiRotation.boolean("MultiRotationReversed").setValue(False)
        aMultiRotation.integer("MultiRotationObjects").setText("n")
        self.aSession.finishOperation()

        self.aMultiRotCopies = aMultiRotation.integer("MultiRotationObjects")

        # create 3D point
        self.aSession.startOperation()
        aPoint3D = self.aDocument.addFeature("Point")
        aPoint3D.string("creation_method").setValue("by_xyz")
        aCoords = geomDataAPI_Point(aPoint3D.attribute("point3d"))
        aCoords.setText("px", "0", "0")
        self.aSession.finishOperation()

        self.aPoint2D = aStartPoint
        self.aPoint3D = aCoords

    def test_rename(self):
        # Rename
        aParam = self.dtParams["x1"]
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.aSession.startOperation()
        aResultAttr.data().setName("a1")
        self.aSession.finishOperation()

        # Check rename in the parameter
        self.assertEqual(aParam.name(), "a1")
        self.assertEqual(aParam.string("variable").value(), "a1")
        self.assertEqual(aResultAttr.data().name(), "a1")
        # Check rename in references
        aParam = self.dtParams["x2"]
        self.assertEqual(aParam.string("expression").value(), "a1 + y1 + 100.0")
        # Check rename in the feature
        self.assertEqual(self.aCircleCenterX.text(), "a1 + 10.0")
        self.assertEqual(self.aCircleCenterY.text(), "a1 + 20.0")
        self.assertEqual(self.aCircleRadius.text(), "a1")
        # Check values
        self.assertEqual(self.anCircleCentr.x(), 160.)
        self.assertEqual(self.anCircleCentr.y(), 170.)
        self.assertEqual(self.aRadiusAttr.value(), 150.)

    def test_rename_integer(self):
        # rename integer parameter
        aParam = self.dtParams["n"]
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.aSession.startOperation()
        aResultAttr.data().setName("m")
        self.aSession.finishOperation()

        # Check rename in the parameter
        self.assertEqual(aParam.name(), "m")
        self.assertEqual(aParam.string("variable").value(), "m")
        self.assertEqual(aResultAttr.data().name(), "m")

        # Check rename in feature
        self.assertEqual(self.aMultiRotCopies.text(), "m")
        # Check corresponding value
        self.assertEqual(self.aMultiRotCopies.value(), 5)

    def test_rename_point(self):
        # rename parameter in point coordinates
        aParam = self.dtParams["px"]
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.aSession.startOperation()
        aResultAttr.data().setName("p")
        self.aSession.finishOperation()

        # Check rename in the parameter
        self.assertEqual(aParam.name(), "p")
        self.assertEqual(aParam.string("variable").value(), "p")
        self.assertEqual(aResultAttr.data().name(), "p")

        # Check rename in feature
        self.assertEqual(self.aPoint2D.textX(), "p")
        self.assertEqual(self.aPoint3D.textX(), "p")
        # Check corresponding value
        self.assertEqual(self.aPoint2D.x(), 50.)
        self.assertEqual(self.aPoint3D.x(), 50.)

    def test_rename_not_unique(self):
        # Rename to not unique name
        aParam = self.dtParams["x1"]
        aParamX1 = aParam
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.aSession.startOperation() # don't finish operation until feature is valid (real case)
        aResultAttr.data().setName("y1")
        # Check rename in the parameter (Expected: renamed, but invalid)
        self.assertEqual(aParam.name(), "y1")
        validators = ModelAPI_Session.get().validators()
        self.assertEqual(validators.validate(aParamX1), False)
        # Check rename in references (Expected: not renamed)
        aParam = self.dtParams["x2"]
        self.assertEqual(aParam.string("expression").value(), "x1 + y1 + 100.0")
        # Check rename in the feature (Expected: not renamed)
        self.assertEqual(self.aCircleCenterX.text(), "x1 + 10.0")
        self.assertEqual(self.aCircleCenterY.text(), "x1 + 20.0")
        self.assertEqual(self.aCircleRadius.text(), "x1")
        # Check values
        self.assertEqual(self.anCircleCentr.x(), 160.)
        self.assertEqual(self.anCircleCentr.y(), 170.)
        self.assertEqual(self.aRadiusAttr.value(), 150.)

        # rename to the correct one, but new
        aResultAttr.data().setName("xx1")
        self.aSession.finishOperation() # feature becomes valid
        # Check rename in the parameter (Expected: renamed)
        self.assertEqual(validators.validate(aParamX1), True)
        self.assertEqual(aParamX1.name(), "xx1")
        self.assertEqual(aParamX1.error(), "")
        # Check rename in references (Expected: renamed)
        aParam = self.dtParams["x2"]
        self.assertEqual(aParam.string("expression").value(), "xx1 + y1 + 100.0")
        # Check rename in the feature (Expected: renamed)
        self.assertEqual(self.aCircleCenterX.text(), "xx1 + 10.0")
        self.assertEqual(self.aCircleCenterY.text(), "xx1 + 20.0")
        self.assertEqual(self.aCircleRadius.text(), "xx1")
        # Check values
        self.assertEqual(self.anCircleCentr.x(), 160.)
        self.assertEqual(self.anCircleCentr.y(), 170.)
        self.assertEqual(self.aRadiusAttr.value(), 150.)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
#=========================================================================
# End of test
#=========================================================================
