# Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
#         finalized by Renaud Nedelec and Sergey Pokhodenko
# Copyright (C) 2014-20xx CEA/DEN, EDF R&D

"""Sketcher interface.
This interface allows to add a sketch
in a part or partset.
The created sketch object provides all the needed methods
for sketch modification and constraint edition.

Example of code:

.. doctest::

   >>> import model
   >>> model.begin()
   >>> partset = model.moduleDocument()
   >>> part = model.addPart(partset).document()
   >>> plane = model.defaultPlane("XOY")
   >>> sketch = model.addSketch(part, plane)
   >>> line = sketch.addLine(0, 0, 0, 1)
   >>> line.endPoint().x()
   0.0
   >>> line.endPoint().y()
   1.0
"""

from ModelAPI import modelAPI_ResultConstruction, featureToCompositeFeature
from GeomDataAPI import geomDataAPI_Point, geomDataAPI_Dir
from GeomAlgoAPI import GeomAlgoAPI_SketchBuilder, ShapeList

from model.sketcher.point import Point
from model.sketcher.line import Line
from model.sketcher.circle import Circle
from model.sketcher.arc import Arc
from model.sketcher.mirror import Mirror
from model.roots import Interface
from model.tools import Selection


def addSketch(document, plane):
    """Add a sketch to a Part or PartSet.

    Arguments:
       document(ModelAPI_Document): part or partset document
       plane(geom.Ax3): plane on wich the sketch is built

    Returns:
       Sketch: sketch object
    """
    feature = featureToCompositeFeature(document.addFeature("Sketch"))
    return Sketch(feature, plane)

class Sketch(Interface):
    """Interface class for Sketch feature."""
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

        # If no arguments are given the attributes of the feature
        # are not Initialized
        if args is not None:
            plane = args[0]
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
        """Add a point to the sketch."""
        if not args:
            raise TypeError("No arguments given")
        point_feature = self._feature.addFeature("SketchPoint")
        return Point(point_feature, *args)

    def addLine(self, *args):
        """Add a line to the sketch.

        .. function:: addLine(name)
        Select an existing line. The line is added to the sketch with a rigid
        constraint (it cannot be modified by the sketch)

        Arguments:
            name(str): name of an existing line

        .. function:: addLine(start, end)
        Create a line by points

        Arguments:
           start(point): start point of the line
           end(point): end point of the line

        .. function:: addLine(start_x, start_y, end_x, end_y)
        Create a line by coordinates

        Arguments:
           start_x(double): start point x coordinate
        """
        if not args:
            raise TypeError("No arguments given")
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
        if not args:
            raise TypeError("No arguments given")
        circle_feature = self._feature.addFeature("SketchCircle")
        return Circle(circle_feature, *args)

    def addArc(self, *args, **kwargs):
        """Add an arc of circle to the sketch and return an arc object.

        Two different syntaxes are allowed:

        .. function:: addArc(center, start, end)

        Arguments:
            center (point): center of the arc
            start (point): start point of the arc
            end (point): end point of the arc

        .. function:: addArc(center_x, center_y, start_x, start_y, end_x, end_y)

        Same as above but with coordinates

        Returns:
            Arc: arc object
        Raises:
            TypeError: if no argument is provided
        """
        if not args:
            raise TypeError("No arguments given")
        arc_feature = self._feature.addFeature("SketchArc")
        return Arc(arc_feature, *args, **kwargs)

    #-------------------------------------------------------------
    #
    # Creation of Geometrical and Dimensional Constraints
    #
    #-------------------------------------------------------------

    def setCoincident(self, p1, p2):
        """Set coincident the two given points and add the corresponding
        constraint to this Sketch."""
        # assert(p1 and p2) NOTE : if an argument is missing python
        # will raise TypeError by itself.
        # It seems better to check only that provided arguments are not
        # None
        if p1 is None or p2 is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintCoincidence")
        self._fillAttribute(constraint.refattr("ConstraintEntityA"), p1)
        self._fillAttribute(constraint.refattr("ConstraintEntityB"), p2)
        self.execute()
        return constraint

    def setParallel(self, l1, l2):
        """Set parallel the two given lines and add the corresponding
        constraint to this Sketch."""
        if l1 is None or l2 is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintParallel")
        constraint.data().refattr("ConstraintEntityA").setObject(l1)
        constraint.data().refattr("ConstraintEntityB").setObject(l2)
        self.execute()
        return constraint

    def setPerpendicular(self, l1, l2):
        """Set perpendicular the two given lines and add the corresponding
        constraint to this Sketch."""
        if l1 is None or l2 is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintPerpendicular")
        constraint.data().refattr("ConstraintEntityA").setObject(l1)
        constraint.data().refattr("ConstraintEntityB").setObject(l2)
        self.execute()
        return constraint

    def setHorizontal(self, line):
        """Set horizontal the given line and add the corresponding
        constraint to this Sketch."""
        if line is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintHorizontal")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        self.execute()
        return constraint

    def setVertical(self, line):
        """Set vertical the given line and add the corresponding
        constraint to this Sketch."""
        if line is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintVertical")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        self.execute()
        return constraint

    def setDistance(self, point, line, length):
        """Set the distance between the given point and line, and add
        the corresponding constraint to this Sketch."""
        if point is None or line is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintDistance")
        if isinstance(line, basestring):
            # Add the edge identified by the given topological name
            # to this Sketch
            line = self.addLine(line).result()
        constraint.data().refattr("ConstraintEntityA").setAttr(point)
        constraint.data().refattr("ConstraintEntityB").setObject(line)
        constraint.data().real("ConstraintValue").setValue(length)
        self.execute()
        return constraint

    def setLength(self, line, length):
        """Set the length of the given line and add the corresponding
        constraint to this Sketch."""
        if line is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintLength")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        self._fillAttribute(constraint.real("ConstraintValue"), length)
        self.execute()
        return constraint

    def setRadius(self, circle, radius):
        """Set the radius of the given circle and add the corresponding
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintRadius")
        self._fillAttribute(constraint.refattr("ConstraintEntityA"), circle)
        self._fillAttribute(constraint.real("ConstraintValue"), radius)
        self.execute()
        return constraint

    def setEqual(self, object_1, object_2):
        """Set the radii of two circles or the length of two lines equal.

        The corresponding constraint is added to the sketch"""
        constraint = self._feature.addFeature("SketchConstraintEqual")
        constraint.data().refattr("ConstraintEntityA").setObject(object_1)
        constraint.data().refattr("ConstraintEntityB").setObject(object_2)
        self.execute()
        return constraint

    def setAngle(self, line_1, line_2, angle):
        """Set the angle between the given 2 lines and add the corresponding
        constraint to the sketch."""
        constraint = self._feature.addFeature("SketchConstraintAngle")
        constraint.data().refattr("ConstraintEntityA").setObject(line_1)
        constraint.data().refattr("ConstraintEntityB").setObject(line_2)
        constraint.data().real("ConstraintValue").setValue(angle)
        self.execute()
        return constraint

    def setTangent(self, object_1, object_2):
        """Set a tangential continuity between two objects
        at their coincidence point."""
        if object_1 is None or object_2 is None:
            raise TypeError("NoneType argument given")
        constraint = self._feature.addFeature("SketchConstraintTangent")
        constraint.data().refattr("ConstraintEntityA").setObject(object_1)
        constraint.data().refattr("ConstraintEntityB").setObject(object_2)
        self.execute()
        return constraint

    def setFillet(self, *args):
        """Set a fillet constraint between the 2 given lines with the given
        filleting radius."""
        assert(args)
        constraint = self._feature.addFeature("SketchConstraintFillet")
        if len(args) == 3:
            line_1, line_2, radius = args
            constraint.data().refattr("ConstraintEntityA").setObject(line_1)
            constraint.data().reflist("ConstraintEntityB").clear()
            constraint.data().reflist("ConstraintEntityB").append(line_2)
        elif len(args) == 2:
            point, radius = args
            self._fillAttribute(constraint.data().refattrlist("ConstraintEntityA"), [point])
            self._fillAttribute(constraint.real("ConstraintValue"), radius)
        self.execute()
        return constraint

    def setRigid(self, object_):
        """Set a rigid constraint on a given object."""
        constraint = self._feature.addFeature("SketchConstraintRigid")
        self._fillAttribute(constraint.refattr("ConstraintEntityA"), object_)
        self.execute()
        return constraint

    #-------------------------------------------------------------
    #
    # Transformation constraints
    #
    #-------------------------------------------------------------

    def addMirror(self, mirror_line, sketch_objects):
        """Add a mirror transformation of the given objects to the sketch.

        This transformation is a constraint.

        :return: interface to the constraint
        :rtype: Mirror object
        """
        mirror_constraint = self._feature.addFeature("SketchConstraintMirror")
        mirror_interface = Mirror(mirror_constraint, mirror_line, sketch_objects)
        self.execute()
        return mirror_interface


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
            self.setCoincident(line_1.endPoint(), line_2.startPoint())
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
            pg[len(coords) - 2].endPoint(), ln.startPoint()
            )
        self.setCoincident(
            ln.endPoint(), pg[0].startPoint()
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
            wire = modelAPI_ResultConstruction(
                self._feature.firstResult()
                ).shape()
        elif len(args) == 1:
            wire = args[0].shape()
        else:
            raise Exception("not yet implemented")
        # TODO: simple version now, should be a list of selected faces
        return [Selection(self.result(), self.buildShape(wire))]

    def buildShape(self, wire):
        """Build the result Shape of this Sketch according to the
        selected geometrical entities."""
        o = self._origin.pnt()
        dx = self._dir_x.dir()
        n = self._norm.dir()

        # The faces are kept otherwise they are destroyed at exit
        faces = ShapeList()
        GeomAlgoAPI_SketchBuilder.createFaces(o, dx, n, wire, faces)
        # TODO: Deal with several faces
        return faces[0]

    def result(self):
        """Returns the result data of this Feature."""
        return self._feature.firstResult()
