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
import model

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
        self.aDocument = self.aSession.moduleDocument()
        self.createParameters()
        self.createFeature()

    def tearDown(self):
        assert(model.checkPythonDump())
        self.aSession.closeAll()

    def createParameters(self):
        ltNames = ["x1", "y1", "x2"]
        ltExpressions = ["150.", "200.", "x1 + y1 + 100.0"]
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
        anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
        aRadiusAttr = aSketchCircle.real("CircleRadius")
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
        anEndPoint.SetValue(0., 100.)
        anOY.selection("External").selectSubShape("EDGE", "OY")
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
        anEndPoint.SetValue(100., 0.)
        anOX.selection("External").selectSubShape("EDGE", "OX")
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
        sel.aCircleRadius = aRadiusConstraintAttr

        self.assertEqual(self.anCircleCentr.x(), 160.)
        self.assertEqual(self.anCircleCentr.y(), 170.)
        self.assertEqual(aRadiusAttr.value(), 150.)

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

    def test_rename_not_unique(self):
        # Rename to not unique name
        aParam = self.dtParams["x1"]
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.aSession.startOperation()
        aResultAttr.data().setName("y1")
        self.aSession.finishOperation()
        # Check rename in the parameter (Expected: not renamed) 
        self.assertEqual(aParam.name(), "x1")
        self.assertEqual(aParam.string("variable").value(), "x1")
        self.assertEqual(aResultAttr.data().name(), "x1")
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

if __name__ == '__main__':
    unittest.main()
#=========================================================================
# End of test
#=========================================================================
