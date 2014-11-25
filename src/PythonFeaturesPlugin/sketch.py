from ModelAPI import *
from GeomDataAPI import *


# Initialization of the Sketch
# ----------------------------

def addTo(doc):
    return modelAPI_CompositeFeature(doc.addFeature("Sketch"))


def setXOYPlane(sketch):
    geomDataAPI_Point(sketch.attribute("Origin")).setValue(0, 0, 0)
    geomDataAPI_Dir(sketch.attribute("DirX")).setValue(1, 0, 0)
    geomDataAPI_Dir(sketch.attribute("DirY")).setValue(0, 1, 0)
    geomDataAPI_Dir(sketch.attribute("Norm")).setValue(0, 0, 1)


# Point geometry
# --------------

def addPoint(x, y, sketch):
    point = sketch.addFeature("SketchPoint")
    geomDataAPI_Point2D(point.attribute("PointCoordindates")).setValue(x, y)
    # Required to get the result, if needed for creating constraints
    point.execute()
    return point


def getGeometry(point):
    return geomDataAPI_Point2D(point.attribute("PointCoordindates"))


# Line geometry
# -------------

def addClosedBrokenLine(coords, sketch):
    c0 = coords[0]
    c1 = coords[1]
    bl = []
    l1 = sketch.addFeature("SketchLine")
    geomDataAPI_Point2D(l1.attribute("StartPoint")).setValue(c0.x(), c0.y())
    geomDataAPI_Point2D(l1.attribute("EndPoint")).setValue(c1.x(), c1.y())
    l1.execute()
    bl.append(l1)
    l0 = l1

    for c2 in coords[2:]:
        l2 = sketch.addFeature("SketchLine")
        geomDataAPI_Point2D(
            l2.attribute("StartPoint")).setValue(c1.x(), c1.y())
        geomDataAPI_Point2D(l2.attribute("EndPoint")).setValue(c2.x(), c2.y())
        l2.execute()
        bl.append(l2)
        constraint = sketch.addFeature("SketchConstraintCoincidence")
        constraint.refattr("ConstraintEntityA").setAttr(
            l1.attribute("EndPoint"))
        constraint.refattr("ConstraintEntityB").setAttr(
            l2.attribute("StartPoint"))
        c1 = c2
        l1 = l2

    if len(coords) > 2:
        l2 = sketch.addFeature("SketchLine")
        geomDataAPI_Point2D(
            l2.attribute("StartPoint")).setValue(c1.x(), c1.y())
        geomDataAPI_Point2D(l2.attribute("EndPoint")).setValue(c0.x(), c0.y())
        l2.execute()
        bl.append(l2)
        constraint = sketch.addFeature("SketchConstraintCoincidence")
        constraint.refattr("ConstraintEntityA").setAttr(
            l1.attribute("EndPoint"))
        constraint.refattr("ConstraintEntityB").setAttr(
            l2.attribute("StartPoint"))

        constraint = sketch.addFeature("SketchConstraintCoincidence")
        constraint.refattr("ConstraintEntityA").setAttr(
            l2.attribute("EndPoint"))
        constraint.refattr("ConstraintEntityB").setAttr(
            l0.attribute("StartPoint"))

    return bl


def addLine(x1, y1, x2, y2, sketch):
    line = sketch.addFeature("SketchLine")
    geomDataAPI_Point2D(line.attribute("StartPoint")).setValue(x1, y1)
    geomDataAPI_Point2D(line.attribute("EndPoint")).setValue(x2, y2)
    # Required to get the result, if needed for creating constraints
    line.execute()
    return line


def getGeometry(line):
    return modelAPI_ResultConstruction(line.firstResult())


def getStartPoint(line):
    return geomDataAPI_Point2D(line.attribute("StartPoint"))


def getEndPoint(line):
    return geomDataAPI_Point2D(line.attribute("EndPoint"))


# Constraints
# -----------

def makeCoincident(p1, p2, sketch):
    constraint = sketch.addFeature("SketchConstraintCoincidence")
    constraint.refattr("ConstraintEntityA").setAttr(p1)
    constraint.refattr("ConstraintEntityB").setAttr(p2)
    return constraint


def makeParallel(l1, l2, sketch):
    constraint = sketch.addFeature("SketchConstraintParallel")
    constraint.refattr("ConstraintEntityA").setObject(l1)
    constraint.refattr("ConstraintEntityB").setObject(l2)
    return constraint


def makePerpendicular(l1, l2, sketch):
    constraint = sketch.addFeature("SketchConstraintPerpendicular")
    constraint.refattr("ConstraintEntityA").setObject(l1)
    constraint.refattr("ConstraintEntityB").setObject(l2)
    return constraint


def makeConstantLength(line, length, sketch):
    constraint = sketch.addFeature("SketchConstraintLength")
    constraint.refattr("ConstraintEntityA").setObject(line)
    constraint.real("ConstraintValue").setValue(length)
    return constraint
