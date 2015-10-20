"""Sketch Feature Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from ModelAPI    import modelAPI_ResultConstruction, featureToCompositeFeature
from GeomDataAPI import geomDataAPI_Point, geomDataAPI_Dir
from GeomAlgoAPI import GeomAlgoAPI_SketchBuilder
from model.sketcher.point import Point
from model.sketcher.line import Line
from model.sketcher.circle import Circle
from model.sketcher.arc import Arc

def addSketch(doc, plane):
    """Add a Sketch feature to the Part or PartSet and return an interface
    on it.
    
    A Sketch object is instanciated with a feature as input parameter
    it provides an interface for manipulation of the feature data.
    :return: interface on the feature
    :rtype: Sketch object"""
    feature = featureToCompositeFeature(doc.addFeature("Sketch"))
    return Sketch(feature, plane)

class Sketch():
    """Interface on a Sketch feature."""
    def __init__(self, feature, plane):
        """Initialize a 2D Sketch on the given plane.
        
        The plane can be defined either by:
        - a 3D axis system (geom.Ax3),
        - an existing face identified by its topological name.
        """
        self._feature = feature
        # Entities used for building the result shape
        self._selection = None
        #   self.resultype ="Face" # Type of Sketch result
        if isinstance(plane, str):
            self.__sketchOnFace(plane)
        else:
            self.__sketchOnPlane(plane)

    def __sketchOnPlane(self, plane):
        o  = plane.location()
        d  = plane.direction()
        dx = plane.xDirection()
        geomDataAPI_Point( 
            self._feature.data().attribute("Origin") 
            ).setValue( o.x(), o.y(), o.z() )
        geomDataAPI_Dir( 
            self._feature.data().attribute("DirX") 
            ).setValue( dx.x(), dx.y(), dx.z() )
        geomDataAPI_Dir( 
            self._feature.data().attribute("Norm") 
            ).setValue( d.x(),  d.y(),  d.z()  )

    def __sketchOnFace(self, plane):
        self._feature.data().selection("External").selectSubShape("FACE", plane)

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
        return Line(line_feature, *args)
    
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
        constraint = self._feature.addFeature("SketchConstraintCoincidence")
        constraint.data().refattr("ConstraintEntityA").setAttr(p1)
        constraint.data().refattr("ConstraintEntityB").setAttr(p2)
        return constraint

    def setParallel(self, l1, l2):
        """Set parallel the two given lines and add the corresponding 
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintParallel")
        constraint.data().refattr("ConstraintEntityA").setObject(l1)
        constraint.data().refattr("ConstraintEntityB").setObject(l2)
        return constraint

    def setPerpendicular(self, l1, l2):
        """Set perpendicular the two given lines and add the corresponding 
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintPerpendicular")
        constraint.data().refattr("ConstraintEntityA").setObject(l1)
        constraint.data().refattr("ConstraintEntityB").setObject(l2)
        return constraint
    
    def setHorizontal(self, line):
        """Set horizontal the given line and add the corresponding 
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintHorizontal")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        return constraint
    
    def setVertical(self, line):
        """Set vertical the given line and add the corresponding 
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintVertical")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        return constraint

    def setDistance(self, point, line, length):
        """Set the distance between the given point and line, and add
        the corresponding constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintDistance")
        if isinstance(line, str):
            # Add the edge identified by the given topological name 
            # to this Sketch
            line = self.addLine(line).result()   
        constraint.data().refattr("ConstraintEntityA").setAttr(point)
        constraint.data().refattr("ConstraintEntityB").setObject(line)
        constraint.data().real("ConstraintValue").setValue(length)
        self._feature.execute()
        return constraint

    def setLength(self, line, length):
        """Set the length of the given line and add the corresponding 
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintLength")
        constraint.data().refattr("ConstraintEntityA").setObject(line)
        constraint.data().real("ConstraintValue").setValue(length)
        self._feature.execute()
        return constraint

    def setRadius(self, circle, radius):
        """Set the radius of the given circle and add the corresponding 
        constraint to this Sketch."""
        constraint = self._feature.addFeature("SketchConstraintRadius")
        constraint.data().refattr("ConstraintEntityA").setObject(circle)
        constraint.data().real("ConstraintValue").setValue(radius)
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


    # Getters

    def selectFace(self, *args):
        """Select the geometrical entities of this Sketch on which 
        the result Face must be built.
        
        When no entity is given, the face is based on all existing 
        geometry of this Sketch.
        """
        #self.resultype ="Face"
        if   len(args) == 0:
            self._selection = modelAPI_ResultConstruction( 
                self._feature.firstResult()).shape()
        elif len(args) == 1:
            self._selection = args[0].shape()
        else:
            raise Exception("not yet implemented")
        return self

    def buildShape(self):
        """Build the result Shape of this Sketch according to the 
        selected geometrical entities."""
        o  = geomDataAPI_Point( self._feature.data().attribute("Origin") ).pnt()
        dx = geomDataAPI_Dir( self._feature.data().attribute("DirX") ).dir()
        n  = geomDataAPI_Dir( self._feature.data().attribute("Norm") ).dir()

        # The faces are kept otherwise they are destroyed at exit
        faces = ShapeList()
        GeomAlgoAPI_SketchBuilder.createFaces(o, dx, n, self._selection, faces)
        #TODO: Deal with several faces 
        return faces[0]

    def result(self):
        """Returns the result data of this Feature."""
        return self._feature.firstResult()
