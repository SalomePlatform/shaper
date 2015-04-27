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

__updated__ = "2015-04-27"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create several parameters.
# 1. Basic parameter definition:
#    x1 = 150.0, y1 = 50.0, cr1 = 100.0, cl = 250.0;
# 2. Check referencing between parameters:
#    x2 = x1 + 100.0, y2 = y1/2.
# 3. Check math module
#    tm = 2 * pi
#=========================================================================
ltNames = ["x1", "y1", "cr1", "cl1"]
ltExpressions = ["150.", "50.", "100.", "250."]
dtParams = {}
aSession.startOperation()
for name, expr in zip(ltNames, ltExpressions):
    aParam = aDocument.addFeature("Parameter")
    aParamName = aParam.string("variable")
    aParamName.setValue(name)
    aParamExpr = aParam.string("expression")
    aParamExpr.setValue(expr)
    dtParams[name] = aParam
aSession.finishOperation()
assert (len(dtParams) == len(ltNames))
# Check results
for name, expr in zip(ltNames, ltExpressions):
    aParam = dtParams[name]
    aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
    assert(aResultAttr.data())
    assert(aResultAttr.data().real("Value"))
    aResultValue = aResultAttr.data().real("Value").value()
    print aResultValue, " == ", float(expr)
    assert(aResultValue == float(expr))

# Check referencing between parameters
aSession.startOperation()
ltNames = ["x2", "y2"]
ltExpressions = ["x1 + 100.0", "y1/2."]
aSession.startOperation()
for name, expr in zip(ltNames, ltExpressions):
    aParam = aDocument.addFeature("Parameter")
    aParamName = aParam.string("variable")
    aParamName.setValue(name)
    aParamExpr = aParam.string("expression")
    aParamExpr.setValue(expr)
    dtParams[name] = aParam
aSession.finishOperation()

aParam = dtParams["x2"]
aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
assert(aResultAttr.data())
assert(aResultAttr.data().real("Value"))
aX2Value = aResultAttr.data().real("Value").value()
assert (aX2Value == 250.)
aParam = dtParams["y2"]
aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
assert(aResultAttr.data())
assert(aResultAttr.data().real("Value"))
aY2Value = aResultAttr.data().real("Value").value()
assert (aY2Value == 25.)
# check math
aSession.startOperation()
aParam = aDocument.addFeature("Parameter")
aParamName = aParam.string("variable")
aParamName.setValue("tm")
aParamExpr = aParam.string("expression")
aParamExpr.setValue("round(2 * pi, 6)")
aSession.finishOperation()
aResultAttr = modelAPI_ResultParameter(aParam.firstResult())
assert(aResultAttr.data())
assert(aResultAttr.data().real("Value"))
aTmValue = aResultAttr.data().real("Value").value()
assert (aTmValue == round(2 * math.pi, 6))
#=========================================================================
# Use parameters to set radius of a circle :
# 1. Create a circle (250., 250), r = 25.
# 2. Set a 'cr1' as text value of radius attribute
#=========================================================================
aSession.startOperation()
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aRadiusAttr = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(250., 250)
aRadiusAttr.setValue(25.)
aSession.finishOperation()
# Apply parameter
aSession.startOperation()
aRadiusAttr.setText("cr1")
aSession.finishOperation()
assert(aRadiusAttr.value() == 100.)
#=========================================================================
# Use parameters for a length constraint on a line:
# 1. Create a line A(10., 10.) - B(-10., -10.)
# 2. Create a length constraint, l = 100;
# 3. Set a 'cl1' as text value of length attribute
#=========================================================================
aSession.startOperation()
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(10., 10.)
aLineEndPoint.setValue(-10., -10.)
aSession.finishOperation()
# Length constraint
aSession.startOperation()
aLengthConstraint = aSketchFeature.addFeature("SketchConstraintLength")
refattrA = aLengthConstraint.refattr("ConstraintEntityA")
refattrA.setObject(modelAPI_ResultConstruction(aSketchLine.firstResult()))
aLengthConstraint.execute()
aSession.finishOperation()

# Apply parameter
aSession.startOperation()
aLengthAttr = aLengthConstraint.real("ConstraintValue")
aLengthAttr.setText("cl1")
aSession.finishOperation()
assert(aLengthAttr.value() == 250.)
#=========================================================================
# End of test
#=========================================================================
