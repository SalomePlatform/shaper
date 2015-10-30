"""Sketch Feature Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from ModelAPI import modelAPI_ResultConstruction, featureToCompositeFeature
from GeomDataAPI import geomDataAPI_Point, geomDataAPI_Dir
from GeomAlgoAPI import GeomAlgoAPI_SketchBuilder, ShapeList

from model.sketcher.point import Point
from model.sketcher.line import Line
from model.sketcher.circle import Circle
from model.sketcher.arc import Arc
from model.roots import Interface
from model.tools import Selection


def addSketch(doc, plane):
    """Add a Sketch feature to the Part or PartSet and return an interface
    on it.

    A Sketch object is instanciated with a feature as input parameter
    it provides an interface for manipulation of the feature data.
    :return: interface on the feature
    :rtype: Sketch"""
    feature = featureToCompositeFeature(doc.addFeature("Sketch"))
    return Sketch(feature, plane)

class Sketch(Interface):
    """Interface on a Sketch feature."""
    def __init__(self, feature, *args):
        """Initialize a 2D Sketch on the given plane.

        The plane can be defined either by:
        - a 3D axis system (geom.Ax3),
        - an existing face identified by its topological name.
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Sketch")

        self._origin = geomDataAPI_Point(
            self._feature.data().attribute("Origin")
            )
        self._dir_x = geomDataAPI_Dir(
            self._feature.data().attribute("DirX")
            )
        self._norm = geomDataAPI_Dir(
            self._feature.data().attribute("Norm")
            )
        self._external = self._feature.data().selection("External")

        assert(self._origin)
        assert(self._dir_x)
        assert(self._norm)
        assert(self._external)

        # Entities used for building the result shape
        self._selection = None

        if not args:
            return

        plane = args[0]

        #   self.resultype ="Face" # Type of Sketch result
        if isinstance(plane, str):
            self.__sketchOnFace(plane)
        else:
            self.__sketchOnPlane(plane)

    def __sketchOnPlane(self, plane):
        """Create the sketch on a plane."""
        origin = plane.location()
        normal = plane.direction()
        x_direction = plane.xDirection()
        self._origin.setValue(origin.x(), origin.y(), origin.z())
        self._norm.setValue(normal.x(), normal.y(), normal.z())
        self._dir_x.setValue(x_direction.x(), x_direction.y(), x_direction.z())

    def __sketchOnFace(self, name):
        """Initialize the sketch on a face given by its name."""
        self._external.selectSubShape("FACE", name)

    #-------------------------------------------------------------
    #
    # Creation of Geometries
    #
    #-------------------------------------------------------------

    def addPoint(self, *args):
        """Add a point to this Sketch."""
        point_feature = self._feature.addFeature("SketchPoint")
        return Point(point_feature, *args)

    def addLine(self, *args):
        """Add a line to this Sketch."""
        line_feature = self._feature.addFeature("SketchLine")
        line_interface = Line(line_feature, *args)
        # if the line is created by name add a rigid constraint
        # to the created line
        if len(args) == 1 and isinstance(args[0], str):
            constraint = self._feature.addFeature("SketchConstraintRigid")
            constraint.refattr("ConstraintEntityA").setObject(
                line_feature.firstResult()
                )
        return line_interface

    def addCircle(self, *args):
        """Add a circle to this Sketch."""
        circle_feature = self._feature.addFeature("SketchCircle")
        return Circle(circle_feature, *args)

    def addArc(self, *args):
        """Add an arc to this Sketch."""
        arc_feature = self._feature.addFeature("SketchArc")
        return Arc(arc_feature, *args)

    #-------------------------------------------------------------
    #
    # Creation of Geometrical and Dimensional Constraints
    #
    #-------------------------------------------------------------

    def setCoincident(self, p1, p2):
        """Set coincident the two given points and add the corresponding
        constraint to this Sketch."""
        assert(p1 and p2)
        constraint = self._feature.addFeature("SketchConstraintCoincidence")
        constraint.data().refattr("ConstraintEntityA").setAttr(p1)
        constraint.data().refattr("ConstraintEntityB").setAttr(p2)
        self._execute()
        return constraint

    def setParallel(self, l1, l2):
        """Set parallel the two given lines and add the corresponding
        constraint to this Sketch."""
        assert(l1 and l2)
        constraint = self._feature.addFeature("SketchConstraintParallel")
        constraint.data().refattr("ConstraintEntityA").setObject(l1)
        constraint.data().refattr("ConstraintEntityB").setObject(l2)
        self._execute()
        return constraint

    def setPerpendicular(self, l1, l2):
        """Set perpendicular the two given lines and add the corresponding
        constraint to this Sketch."""
        assert(l1 and l2)
        constraint = self._feature.addFeature("SketchConstraintPerpendicular")
        constraint.data().refattr("ConstraintEntityA").setObject(l1)
        constraint.data().refattr("ConstraintEntityB").setObject(l2)
        self._execute()
        return constraint

    def setHorizontal(self, line):
        """Set horizontal the given line and add the corresponding
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintHorizontal")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        self._execute()
        return constraint

    def setVertical(self, line):
        """Set vertical the given line and add the corresponding
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintVertical")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        self._execute()
        return constraint

    def setDistance(self, point, line, length):
        """Set the distance between the given point and line, and add
        the corresponding constraint to this Sketch."""
        assert(point and line)
        constraint = self._feature.addFeature("SketchConstraintDistance")
        if isinstance(line, basestring):
            # Add the edge identified by the given topological name
            # to this Sketch
            line = self.addLine(line).result()
            assert(line)
        constraint.data().refattr("ConstraintEntityA").setAttr(point)
        constraint.data().refattr("ConstraintEntityB").setObject(line)
        constraint.data().real("ConstraintValue").setValue(length)
        self._execute()
        return constraint

    def setLength(self, line, length):
        """Set the length of the given line and add the corresponding
        constraint to this Sketch."""
        assert(line)
        constraint = self._feature.addFeature("SketchConstraintLength")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        constraint.data().real("ConstraintValue").setValue(length)
        self._execute()
        return constraint

    def setRadius(self, circle, radius):
        """Set the radius of the given circle and add the corresponding
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintRadius")
        constraint.data().refattr("ConstraintEntityA").setObject(circle)
        constraint.data().real("ConstraintValue").setValue(radius)
        self._execute()
        return constraint

    def setEqual(self, object_1, object_2):
        """Set the radii of two circles or the length of two lines equal.

        The corresponding constraint is added to the sketch"""
        constraint = self._feature.addFeature("SketchConstraintEqual")
        constraint.data().refattr("ConstraintEntityA").setObject(object_1)
        constraint.data().refattr("ConstraintEntityB").setObject(object_2)
        self._execute()
        return constraint

    def setAngle(self, line_1, line_2, angle):
        """Set the angle between the given 2 lines and add the corresponding
        constraint to the sketch."""
        constraint = self._feature.addFeature("SketchConstraintAngle")
        constraint.data().refattr("ConstraintEntityA").setObject(line_1)
        constraint.data().refattr("ConstraintEntityB").setObject(line_2)
        constraint.data().real("ConstraintValue").setValue(angle)
        self._execute()
        return constraint

    def setTangent(self, object_1, object_2):
        """Set a tangential continuity between two objects
        at their coincidence point."""
        constraint = self._feature.addFeature("SketchConstraintTangent")
        constraint.data().refattr("ConstraintEntityA").setObject(object_1)
        constraint.data().refattr("ConstraintEntityB").setObject(object_2)
        self._execute()
        return constraint

    def setFillet(self, line_1, line_2, radius):
        """Set a fillet constraint between the 3 given lines with the given
        filleting radius."""
        constraint = self._feature.addFeature("SketchConstraintFillet")
        constraint.data().refattr("ConstraintEntityA").setObject(line_1)
        constraint.data().refattr("ConstraintEntityB").setObject(line_2)
        constraint.data().real("ConstraintValue").setValue(radius)
        self._execute()
        return constraint

    #-------------------------------------------------------------
    #
    # Edition of Dimensional Constraints
    #
    #-------------------------------------------------------------

    def setValue(self, constraint, value):
        """Modify the value of the given dimensional constraint."""
        constraint.data().real("ConstraintValue").setValue(value)

    #-------------------------------------------------------------
    #
    # Macro functions combining geometry creation and constraints
    #
    #-------------------------------------------------------------

    def addPolyline(self, *coords):
        """Add a poly-line to this Sketch.

        The end of consecutive segments are defined as coincident.
        """
        c0 = coords[0]
        c1 = coords[1]
        polyline = []
        line_1 = self.addLine(c0, c1)
        polyline.append(line_1)
        # Adding and connecting next lines
        for c2 in coords[2:]:
            line_2 = self.addLine(c1, c2)
            self.setCoincident(line_1.endPointData(), line_2.startPointData())
            polyline.append(line_2)
            c1 = c2
            line_1 = line_2
        return polyline

    def addPolygon(self, *coords):
        """Add a polygon to this Sketch.

        The end of consecutive segments are defined as coincident.
        """
        pg = self.addPolyline(*coords)
        # Closing the poly-line supposed being defined by at least 3 points
        c0 = coords[0]
        cn = coords[len(coords) - 1]
        ln = self.addLine(cn, c0)
        self.setCoincident(
            pg[len(coords) - 2].endPointData(), ln.startPointData()
            )
        self.setCoincident(
            ln.endPointData(), pg[0].startPointData()
            )
        pg.append(ln)
        return pg

    #-------------------------------------------------------------
    #
    # Getters
    #
    #-------------------------------------------------------------

    def selectFace(self, *args):
        """Select the geometrical entities of this Sketch on which
        the result Face must be built.

        When no entity is given, the face is based on all existing
        geometry of this Sketch.
        """
        if len(args) == 0:
            self._selection = modelAPI_ResultConstruction(
                self._feature.firstResult()
                ).shape()
        elif len(args) == 1:
            self._selection = args[0].shape()
        else:
            raise Exception("not yet implemented")
        # TODO: simple version now, should be a list of selected faces
        return [Selection(self.result(), self.buildShape())]

    def buildShape(self):
        """Build the result Shape of this Sketch according to the
        selected geometrical entities."""
        o = self._origin.pnt()
        dx = self._dir_x.dir()
        n = self._norm.dir()

        # The faces are kept otherwise they are destroyed at exit
        faces = ShapeList()
        GeomAlgoAPI_SketchBuilder.createFaces(o, dx, n, self._selection, faces)
        # TODO: Deal with several faces
        return faces[0]

    def result(self):
        """Returns the result data of this Feature."""
        return self._feature.firstResult()
