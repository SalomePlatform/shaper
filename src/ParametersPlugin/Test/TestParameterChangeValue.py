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

__updated__ = "2015-04-27"

#=========================================================================
# Create several parameters and a feature.
# 1. Basic parameter definition:
#    x1 = 150.0, y1 = 200.0
# 2. Referencing between parameters:
#    x2 = x1 + y1 + 100.0
# 3. Referencing in feature SketchCircle
#    CircleCenter = (x1 + 10.0, x1 + 20.0), CircleRadius = x1
# 3. Change value
#    x1 = 200.0
#=========================================================================
class TestParameterRename(unittest.TestCase):
    def setUp(self):
        self.aSession = ModelAPI_Session.get()
        self.aDocument = self.aSession.moduleDocument()
        self.createParameters()
        self.createFeature()

    def tearDown(self):
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
        anCircleCentr.setText("x1 + 10.0", "x1 + 20.0")
        aRadiusAttr.setText("x1")
        self.aSession.finishOperation()

        self.anCircleCentr = anCircleCentr
        self.aRadiusAttr = aRadiusAttr

        self.assertEqual(self.anCircleCentr.x(), 160.)
        self.assertEqual(self.anCircleCentr.y(), 170.)
        self.assertEqual(aRadiusAttr.value(), 150.)

    def test_change_value(self):
        aParam = self.dtParams["x1"]
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.aSession.startOperation()
        aParam.string("expression").setValue("200.0")
        self.aSession.finishOperation()

        # Check value of the parameter 
        self.assertEqual(aResultAttr.data().real("Value").value(), 200.)
        # Check value of references
        aParam = self.dtParams["x2"]
        aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
        self.assertEqual(aResultAttr.data().real("Value").value(), 500.)
        # Check value in the feature
        self.assertEqual(self.anCircleCentr.x(), 210.)
        self.assertEqual(self.anCircleCentr.y(), 220.)
        self.assertEqual(self.aRadiusAttr.value(), 200.)

if __name__ == '__main__':
    unittest.main()
#=========================================================================
# End of test
#=========================================================================
