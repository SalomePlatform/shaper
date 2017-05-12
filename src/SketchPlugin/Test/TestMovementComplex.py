"""
    Test moving the entities in the complex sketch by the example of a clamp
"""

from salome.shaper import model
from SketchAPI import *
from GeomAPI import *
import math

TOLERANCE = 1.e-7

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Create sketch representing main part of clamp
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(20, 165, 175, 165)
SketchLine_2 = Sketch_1.addLine(175, 165, 175, 120)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(175, 120, 80, 20)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(80, 20, 20, 20)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(20, 20, 20, 165)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 155)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_2.result(), SketchLine_1.result())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_1.result(), SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 45)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchLine_1.result(), 145)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_4.startPoint(), SketchLine_3.startPoint(), 95)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchPoint_1.coordinates(), SketchLine_5.endPoint(), 20)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchPoint_1.coordinates(), SketchLine_5.endPoint(), 165)
model.do()

# =============================================================================
# Test 1. Create fillet and move fillet arc
# =============================================================================
Sketch_1.setFillet(SketchLine_5.startPoint())
SketchArc_1 = SketchAPI_Arc(model.lastSubFeature(Sketch_1, "SketchArc"))
model.do()
anArcCenter = [SketchArc_1.center().x(), SketchArc_1.center().y()]
anArcStart  = [SketchArc_1.startPoint().x(), SketchArc_1.startPoint().y()]
anArcEnd    = [SketchArc_1.endPoint().x(), SketchArc_1.endPoint().y()]
# move arc dragging center
delta = [1., 1.]
for i in range(1, 10):
    anArcCenter = [anArcCenter[0] + delta[0], anArcCenter[1] + delta[1]]
    Sketch_1.move(SketchArc_1.center(), anArcCenter[0], anArcCenter[1])
    model.do()
    model.assertArcValidity(SketchArc_1)
    model.assertPoint(SketchArc_1.center(), anArcCenter)
    assert(SketchArc_1.startPoint().x() == anArcStart[0])
    assert(SketchArc_1.endPoint().y() == anArcEnd[1])
    if i == 5:
        delta = [-1., -1.]
# move arc dragging start point
delta = [1., 1.]
aPosition = anArcStart
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchArc_1.startPoint(), aPosition[0], aPosition[1])
    model.do()
    model.assertArcValidity(SketchArc_1)
    assert(SketchArc_1.startPoint().x() == anArcStart[0])
    assert(SketchArc_1.endPoint().y() == anArcEnd[1])
    if i == 5:
        delta = [-1., -1.]
# move arc dragging end point
delta = [1., 1.]
aPosition = anArcEnd
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchArc_1.endPoint(), aPosition[0], aPosition[1])
    model.do()
    model.assertArcValidity(SketchArc_1)
    assert(SketchArc_1.startPoint().x() == anArcStart[0])
    assert(SketchArc_1.endPoint().y() == anArcEnd[1])
    if i == 5:
        delta = [-1., -1.]

# =============================================================================
# Test 2. Create circle and move it near its potential position
# =============================================================================
aCircleCenter = [0, 0]
aCircleRadius = 5
SketchCircle_1 = Sketch_1.addCircle(aCircleCenter[0], aCircleCenter[1], aCircleRadius)
model.do()
# move center
delta = [3., 2.]
for i in range(1, 15):
    aCircleCenter = [aCircleCenter[0] + delta[0], aCircleCenter[1] + delta[1]]
    Sketch_1.move(SketchCircle_1.center(), aCircleCenter[0], aCircleCenter[1])
    model.do()
    model.assertPoint(SketchCircle_1.center(), aCircleCenter)
    assert(SketchCircle_1.radius().value() == aCircleRadius)
# move circle by dragging edge
aPosition = [aCircleCenter[0] + aCircleRadius, aCircleCenter[1]]
delta = [1., 1.]
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchCircle_1.defaultResult(), aPosition[0], aPosition[1])
    model.do()
    model.assertPoint(SketchCircle_1.center(), aCircleCenter)
    assert(math.fabs(SketchCircle_1.radius().value() - model.distancePointPoint(aPosition, aCircleCenter)) < TOLERANCE)

# =============================================================================
# Test 3. Constrain center of circle and move it again
# =============================================================================
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchCircle_1.center(), SketchArc_1.startPoint(), -30)
model.do()
# move center
delta = [1., 1.]
aCircleCenter = [SketchCircle_1.center().x(), SketchCircle_1.center().y()]
aPosition = aCircleCenter
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchCircle_1.center(), aPosition[0], aPosition[1])
    model.do()
    assert(SketchCircle_1.center().x() == aCircleCenter[0])
    if i == 5:
        delta = [-1., -1.]
# move circle by dragging edge
aCircleCenter = [SketchCircle_1.center().x(), SketchCircle_1.center().y()]
aPosition = [aCircleCenter[0] + aCircleRadius, aCircleCenter[1]]
delta = [1., 1.]
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchCircle_1.defaultResult(), aPosition[0], aPosition[1])
    model.do()
    model.assertPoint(SketchCircle_1.center(), aCircleCenter)
    assert(math.fabs(SketchCircle_1.radius().value() - model.distancePointPoint(aPosition, aCircleCenter)) < TOLERANCE)
# set radius of the circle
aCircleRadius = 17.5
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], aCircleRadius)
model.do()
# move center
delta = [1., 1.]
aCircleCenter = [SketchCircle_1.center().x(), SketchCircle_1.center().y()]
aPosition = aCircleCenter
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchCircle_1.center(), aPosition[0], aPosition[1])
    model.do()
    assert(SketchCircle_1.center().x() == aCircleCenter[0])
    assert(SketchCircle_1.radius().value() == aCircleRadius)
    if i == 5:
        delta = [-1., -1.]
# move circle by dragging edge
aCircleCenter = [SketchCircle_1.center().x(), SketchCircle_1.center().y()]
aPosition = [aCircleCenter[0] + aCircleRadius, aCircleCenter[1]]
delta = [1., 1.]
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchCircle_1.defaultResult(), aPosition[0], aPosition[1])
    model.do()
    assert(SketchCircle_1.center().x() == aCircleCenter[0])
    assert(SketchCircle_1.radius().value() == aCircleRadius)

# =============================================================================
# Test 4. Set centers of circle and arc coincident and move circle
# =============================================================================
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchArc_1.center())
model.do()
assert(model.dof(Sketch_1) == 0)
# move circle
aCircleCenter = [SketchCircle_1.center().x(), SketchCircle_1.center().y()]
aPosition = [aCircleCenter[0], aCircleCenter[1] + aCircleRadius]
delta = [0., 1.]
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchCircle_1.defaultResult(), aPosition[0], aPosition[1])
    model.do()
    model.assertPoint(SketchCircle_1.center(), aCircleCenter)
    assert(SketchCircle_1.radius().value() == aCircleRadius)
assert(model.dof(Sketch_1) == 0)

# Extrude main part of clamp
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2r-SketchLine_3r-SketchLine_4r-SketchLine_5r-SketchArc_1_2f-SketchCircle_1_2r")], model.selection(), 10, 0)
model.do()


# Create sketch representing orthogonal part of clamp
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchRectangle_1 = Sketch_2.addRectangle(20, 0, 100, 50)
[SketchLine_6, SketchLine_7, SketchLine_8, SketchLine_9] = SketchRectangle_1.lines()
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"))
SketchLine_10 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_7).startPoint(), SketchAPI_Line(SketchLine_10).startPoint())
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_8.result(), SketchLine_10.result())
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_7.result(), 80)
model.do()
assert(model.dof(Sketch_2) == 0)

# Create auxiliary lines for further holes
SketchLine_11 = Sketch_2.addLine(20, 10, 175, 10)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_7.result())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_9.result())
SketchConstraintParallel_2 = Sketch_2.setParallel(SketchLine_11.result(), SketchLine_8.result())
SketchLine_12 = Sketch_2.addLine(150, 0, 150, 80)
SketchLine_12.setAuxiliary(True)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_12.startPoint(), SketchLine_6.result())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_8.result())
SketchConstraintPerpendicular_2 = Sketch_2.setPerpendicular(SketchLine_11.result(), SketchLine_12.result())
model.do()
assert(model.dof(Sketch_2) == 2)

# =============================================================================
# Test 5. Create circular hole and move it
# =============================================================================
SketchCircle_2 = Sketch_2.addCircle(50, 10, 10)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchLine_11.result())
model.do()
# move center
delta = [2., 1.]
aCircleCenter = [SketchCircle_2.center().x(), SketchCircle_2.center().y()]
aCircleRadius = SketchCircle_2.radius().value()
for i in range(1, 10):
    aCircleCenter = [aCircleCenter[0] + delta[0], aCircleCenter[1] + delta[1]]
    Sketch_2.move(SketchCircle_2.center(), aCircleCenter[0], aCircleCenter[1])
    model.do()
    model.assertPoint(SketchCircle_2.center(), aCircleCenter)
    assert(aCircleRadius == SketchCircle_2.radius().value())
# move circle by dragging edge
aCircleCenter = [SketchCircle_2.center().x(), SketchCircle_2.center().y()]
aPosition = [aCircleCenter[0] + aCircleRadius, aCircleCenter[1]]
delta = [0., 1.]
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_1.move(SketchCircle_2.defaultResult(), aPosition[0], aPosition[1])
    model.do()
    model.assertPoint(SketchCircle_2.center(), aCircleCenter)
    assert(math.fabs(SketchCircle_2.radius().value() - model.distancePointPoint(aPosition, aCircleCenter)) < TOLERANCE)

SketchConstraintDistanceHorizontal_4 = Sketch_2.setHorizontalDistance(SketchLine_11.startPoint(), SketchCircle_2.center(), 21.5)
model.do()
assert(model.dof(Sketch_2) == 3)

# =============================================================================
# Test 6. Create oval hole and move it
# =============================================================================
anArcRadius = 5
anArcCenter = [SketchLine_11.endPoint().x() + anArcRadius, SketchCircle_2.center().y()]
SketchArc_2 = Sketch_2.addArc(anArcCenter[0], anArcCenter[1], anArcCenter[0], anArcCenter[1] + anArcRadius, anArcCenter[0], anArcCenter[1] - anArcRadius, True)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchArc_2.center(), SketchLine_11.result())
SketchLine_13 = Sketch_2.addLine(anArcCenter[0], anArcCenter[1] + anArcRadius, SketchLine_12.startPoint().x(), anArcCenter[1] + anArcRadius)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchArc_2.startPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_12.result())
SketchConstraintPerpendicular_3 = Sketch_2.setPerpendicular(SketchLine_13.result(), SketchLine_12.result())
SketchLine_14 = Sketch_2.addLine(anArcCenter[0], anArcCenter[1] - anArcRadius, SketchLine_12.startPoint().x(), anArcCenter[1] - anArcRadius)
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_12.result())
SketchConstraintPerpendicular_4 = Sketch_2.setPerpendicular(SketchLine_14.result(), SketchLine_12.result())
SketchConstraintTangent_1 = Sketch_2.setTangent(SketchArc_2.results()[1], SketchLine_13.result())
SketchConstraintTangent_2 = Sketch_2.setTangent(SketchArc_2.results()[1], SketchLine_14.result())
model.do()
assert(model.dof(Sketch_2) == 5)
# move center of arc
delta = [-2., 1.]
anArcCenter = [SketchArc_2.center().x(), SketchArc_2.center().y()]
for i in range(1, 10):
    anArcCenter = [anArcCenter[0] + delta[0], anArcCenter[1] + delta[1]]
    Sketch_2.move(SketchArc_2.center(), anArcCenter[0], anArcCenter[1])
    model.do()
    model.assertArc(SketchArc_2, anArcCenter, [], [])

# Complete the oval hole
SketchConstraintMirror_1_objects = [SketchLine_13.result(), SketchArc_2.results()[1], SketchLine_14.result()]
SketchConstraintMirror_1 = Sketch_2.addMirror(SketchLine_12.result(), SketchConstraintMirror_1_objects)
[SketchLine_15, SketchArc_3, SketchLine_16] = SketchConstraintMirror_1.mirrored()
model.do()

# =============================================================================
# Test 7. Set arc and circle equal radii and move arc
# =============================================================================
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchCircle_2.results()[1], SketchArc_2.results()[1])
model.do()
# move center of arc
delta = [-1., -1.]
anArcCenter = [SketchArc_2.center().x(), SketchArc_2.center().y()]
aCircleCenter = [SketchCircle_2.center().x(), SketchCircle_2.center().y()]
for i in range(1, 10):
    anArcCenter = [anArcCenter[0] + delta[0], anArcCenter[1] + delta[1]]
    Sketch_2.move(SketchArc_2.center(), anArcCenter[0], anArcCenter[1])
    model.do()
    model.assertArc(SketchArc_2, anArcCenter, [], [])
    model.assertPoint(SketchCircle_2.center(), [aCircleCenter[0], anArcCenter[1]])
    anArcRadius = model.distancePointPoint(SketchArc_2.center(), SketchArc_2.startPoint())
    assert(math.fabs(SketchCircle_2.radius().value() - anArcRadius) < TOLERANCE)
# move arc by dragging edge
aCircleCenter = [SketchCircle_2.center().x(), SketchCircle_2.center().y()]
anArcCenter = [SketchArc_2.center().x(), SketchArc_2.center().y()]
aPosition = [anArcCenter[0] + anArcRadius, anArcCenter[1]]
delta = [1., 1.]
for i in range(1, 10):
    aPosition = [aPosition[0] + delta[0], aPosition[1] + delta[1]]
    Sketch_2.move(SketchArc_2.defaultResult(), aPosition[0], aPosition[1])
    model.do()
    model.assertArc(SketchArc_2, anArcCenter, [], [])
    model.assertPoint(SketchCircle_2.center(), aCircleCenter)
    anArcRadius = model.distancePointPoint(SketchArc_2.center(), SketchArc_2.startPoint())
    assert(math.fabs(SketchCircle_2.radius().value() - anArcRadius) < TOLERANCE)
    if i == 5:
        delta = [-1., -1.]

# Fix all DOF in Sketch_2
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchArc_2.center(), SketchLine_9.result(), 21.5)
SketchConstraintLength_4 = Sketch_2.setLength(SketchLine_13.result(), 3.5)
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_2.results()[1], 6.5)
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchCircle_2.center(), SketchLine_8.result(), 30)
model.do()

Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_6r-SketchLine_7r-SketchLine_8r-SketchLine_9r-SketchCircle_2_2r-SketchArc_2_2r-SketchLine_13r-SketchLine_14f-SketchLine_15f-SketchArc_3_2r-SketchLine_16r")], model.selection(), -165, 155)
Boolean_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_2_1")])

Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Boolean_1_1/Modified_8"))
SketchLine_17 = Sketch_3.addLine(145, 0, 165, 0)
SketchPoint_2 = Sketch_3.addPoint(model.selection("VERTEX", "Boolean_1_1/Modified_7&Boolean_1_1/Modified_6&Boolean_1_1/Modified_2"))
SketchConstraintCoincidence_23 = Sketch_3.setCoincident(SketchLine_17.endPoint(), SketchPoint_2.result())
SketchLine_18 = Sketch_3.addLine(165, 0, 165, -20)
SketchLine_19 = Sketch_3.addLine(165, -20, 145, -20)
SketchLine_20 = Sketch_3.addLine(145, -20, 145, 0)
SketchConstraintCoincidence_24 = Sketch_3.setCoincident(SketchLine_20.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_25 = Sketch_3.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_26 = Sketch_3.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_27 = Sketch_3.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchConstraintHorizontal_4 = Sketch_3.setHorizontal(SketchLine_17.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_18.result())
SketchConstraintHorizontal_5 = Sketch_3.setHorizontal(SketchLine_19.result())
SketchConstraintVertical_5 = Sketch_3.setVertical(SketchLine_20.result())
SketchArc_4 = Sketch_3.addArc(145, -20, 165, -20, 145, 0, False)
SketchConstraintCoincidence_28 = Sketch_3.setCoincident(SketchLine_19.endPoint(), SketchArc_4.center())
SketchConstraintCoincidence_29 = Sketch_3.setCoincident(SketchLine_18.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_30 = Sketch_3.setCoincident(SketchArc_4.endPoint(), SketchLine_20.endPoint())
SketchConstraintRadius_3 = Sketch_3.setRadius(SketchArc_4.results()[1], 20)
SketchArc_5 = Sketch_3.addArc(145, -20, 155, -20, 145, -10, False)
SketchConstraintCoincidence_31 = Sketch_3.setCoincident(SketchLine_19.endPoint(), SketchArc_5.center())
SketchConstraintCoincidence_32 = Sketch_3.setCoincident(SketchLine_20.result(), SketchArc_5.endPoint())
SketchConstraintCoincidence_33 = Sketch_3.setCoincident(SketchArc_5.startPoint(), SketchLine_19.result())
SketchLine_21 = Sketch_3.addLine(model.selection("EDGE", "Boolean_1_1/Modified_12&Boolean_1_1/Modified_11"))
SketchConstraintTangent_5 = Sketch_3.setTangent(SketchArc_5.results()[1], SketchLine_21.result())
model.do()

ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_17r-SketchLine_18r-SketchArc_4_2r")], model.selection(), model.selection("FACE", "Boolean_1_1/Modified_5"), 0, model.selection(), 0, [model.selection("SOLID", "Boolean_1_1")])
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_19r-SketchLine_20r-SketchArc_4_2f-SketchArc_5_2r")], model.selection(), model.selection("FACE", "ExtrusionCut_1_1/Modfied_7"), 0, model.selection(), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
model.do()

model.end()

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [140])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [280])
model.testResultsVolumes(ExtrusionFuse_1, [260653.824775497108930721879005432])

assert(model.checkPythonDump())
