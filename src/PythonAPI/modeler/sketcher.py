"""Sketch Feature Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from ModelAPI    import *
from GeomDataAPI import *
from GeomAlgoAPI import *


class Sketch():
  """ A class of Sketcher"""

  def __init__(self, doc, plane):
    """Initializes a 2D Sketch on the given plane and adds the Sketch to the given Part or Partset.
	The plane can be defined either by:
	- a 3D axis system (geom.Ax3),
	- an existing face identified by its topological name.
    """
    ### Create a feature object
    self.my = featureToCompositeFeature( doc.addFeature("Sketch") )
    ### Entities used for building the result shape
    self.selection = None         
#   self.resultype ="Face"        # Type of Sketch result
    if isinstance(plane, str):
      self.__sketchOnFace(doc, plane)
    else:
      self.__sketchOnPlane(doc, plane)

  def __sketchOnPlane (self, doc, plane):
    o  = plane.location()
    d  = plane.direction()
    dx = plane.xDirection()
    geomDataAPI_Point( self.my.data().attribute("Origin") ).setValue( o.x(), o.y(), o.z() )
    geomDataAPI_Dir( self.my.data().attribute("DirX") ).setValue( dx.x(), dx.y(), dx.z() )
    geomDataAPI_Dir( self.my.data().attribute("Norm") ).setValue( d.x(),  d.y(),  d.z()  )

  def __sketchOnFace (self, doc, plane):
    self.my.data().selection("External").selectSubShape("FACE", plane)


# Creation of Geometries

  def addPoint (self, *args):
    """Adds a point to this Sketch."""
    return Point(self.my, *args)

  def addLine (self, *args):
    """Adds a line to this Sketch."""
    return Line(self.my, *args)

  def addPolyline (self, *coords):
    """Adds a poly-line to this Sketch.
    The end of consecutive segments are defined as coincident.
    """
    c0 = coords[0]
    c1 = coords[1]
    pl = []
    l1 = self.addLine(c0, c1)
    pl.append(l1)
    # Adding and connecting next lines
    for c2 in coords[2:]:
      l2 = self.addLine(c1, c2)
      self.setCoincident( l1.endPointData(), l2.startPointData() )
      pl.append(l2)
      c1 = c2
      l1 = l2
    return pl

  def addPolygon (self, *coords):
    """Adds a polygon to this Sketch.
    The end of consecutive segments are defined as coincident.
    """
    pg = self.addPolyline(*coords)
	# Closing the poly-line supposed being defined by at least 3 points
    c0 = coords[0]
    cn = coords[len(coords)-1]
    ln = self.addLine(cn, c0)
    self.setCoincident( pg[len(coords)-2].endPointData(), ln.startPointData() )
    self.setCoincident( ln.endPointData(), pg[0].startPointData() )
    pg.append(ln)
    return pg

  def addCircle (self, *args):
    """Adds a circle to this Sketch."""
    return Circle(self.my, *args)


# Creation of Geometrical and Dimensional Constraints

  def setCoincident (self, p1, p2):
    """Sets coincident the two given points and adds the corresponding constraint to this Sketch."""
    constraint = self.my.addFeature("SketchConstraintCoincidence")
    constraint.data().refattr("ConstraintEntityA").setAttr(p1)
    constraint.data().refattr("ConstraintEntityB").setAttr(p2)
    return constraint

  def setParallel (self, l1, l2):
    """Sets parallel the two given lines and adds the corresponding constraint to this Sketch."""
    constraint = self.my.addFeature("SketchConstraintParallel")
    constraint.data().refattr("ConstraintEntityA").setObject(l1)
    constraint.data().refattr("ConstraintEntityB").setObject(l2)
    return constraint

  def setPerpendicular (self, l1, l2):
    """Sets perpendicular the two given lines and adds the corresponding constraint to this Sketch."""
    constraint = self.my.addFeature("SketchConstraintPerpendicular")
    constraint.data().refattr("ConstraintEntityA").setObject(l1)
    constraint.data().refattr("ConstraintEntityB").setObject(l2)
    return constraint

  def setDistance (self, point, line, length):
    """Sets the distance between the given point and line, and adds the corresponding constraint to this Sketch."""
    constraint = self.my.addFeature("SketchConstraintDistance")
    if isinstance(line, str):
      line = self.addLine(line).result()   # Adds the edge identified by the given topological name to this Sketch
    constraint.data().refattr("ConstraintEntityA").setAttr(point)
    constraint.data().refattr("ConstraintEntityB").setObject(line)
    constraint.data().real("ConstraintValue").setValue(length)
    self.my.execute()
    return constraint

  def setLength (self, line, length):
    """Sets the length of the given line and adds the corresponding constraint to this Sketch."""
    constraint = self.my.addFeature("SketchConstraintLength")
    constraint.data().refattr("ConstraintEntityA").setObject(line)
    constraint.data().real("ConstraintValue").setValue(length)
    self.my.execute()
    return constraint

  def setRadius (self, circle, radius):
    """Sets the radius of the given circle and adds the corresponding constraint to this Sketch."""
    constraint = self.my.addFeature("SketchConstraintRadius")
    constraint.data().refattr("ConstraintEntityA").setObject(circle)
    constraint.data().real("ConstraintValue").setValue(radius)
    return constraint


# Edition of Dimensional Constraints

  def setValue (self, constraint, value):
    """Modifies the value of the given dimensional constraint."""
    constraint.data().real("ConstraintValue").setValue(value)


# Getters

  def selectFace (self, *args):
    """Selects the geometrical entities of this Sketch on which the result Face must be built.
	When no entity is given, the face is based on all existing geometry of this Sketch.
	"""
    #self.resultype ="Face"
    if   len(args) == 0:
      self.selection = modelAPI_ResultConstruction( self.my.firstResult() ).shape()
    elif len(args) == 1:
      self.selection = args[0].shape()
    else:
      raise Exception("not yet implemented")
    return self

  def buildShape (self):
    """Builds the result Shape of this Sketch according to the selected geometrical entities."""
    o  = geomDataAPI_Point( self.my.data().attribute("Origin") ).pnt()
    dx = geomDataAPI_Dir( self.my.data().attribute("DirX") ).dir()
    n  = geomDataAPI_Dir( self.my.data().attribute("Norm") ).dir()

    faces = ShapeList()      # The faces are kept otherwise they are destroyed at exit
    GeomAlgoAPI_SketchBuilder.createFaces(o, dx, n, self.selection, faces)
#TODO: Deal with several faces 
    return faces[0]

  def result (self):
    """Returns the result data of this Feature."""
    return self.my.firstResult()


# Class definitions of Sketch features

class Point():
  """A class which represents a Point object"""

  def __init__(self, sketch, x, y):
    """Constructor"""
    ### Create the feature
    self.my = sketch.addFeature("SketchPoint")
    geomDataAPI_Point2D( self.my.data().attribute("PointCoordinates") ).setValue(x, y)
    self.my.execute()

  def pointData (self):
    """Returns points attribute"""
    return geomDataAPI_Point2D( self.my.data().attribute("PointCoordinates") )

  def result (self):
    """Returns result object"""
    return self.my.firstResult()


class Line():
  """A class which represents a Line object"""

  def __init__(self, sketch, *args):
    """Constructor"""
    ### Create the feature
    self.my = sketch.addFeature("SketchLine")
    if   len(args) == 4:
      self.__createByCoordinates(*args)
    elif len(args) == 2:
      self.__createByPoints(*args)
    elif len(args) == 1:
	  self.__createByName(sketch, *args)
    else:
      raise Exception("cannot create the Line")

  def __createByCoordinates(self, x1, y1, x2, y2):
    """Initialise the feature by coordinates"""
    geomDataAPI_Point2D( self.my.data().attribute("StartPoint") ).setValue(x1, y1)
    geomDataAPI_Point2D( self.my.data().attribute("EndPoint") ).setValue(x2, y2)
    self.my.execute()

  def __createByPoints(self, p1, p2):
    """Initialise the feature by point objects"""
    geomDataAPI_Point2D( self.my.data().attribute("StartPoint") ).setValue(p1.x(), p1.y())
    geomDataAPI_Point2D( self.my.data().attribute("EndPoint") ).setValue(p2.x(), p2.y())
    self.my.execute()

  def __createByName(self, sketch, name):
    """Initialise the feature by name of edge"""
    self.my.data().selection("External").selectSubShape("EDGE", name)
    self.my.execute()
    rigid = sketch.addFeature("SketchConstraintRigid")
    rigid.refattr("ConstraintEntityA").setObject( self.my.firstResult() )

  def startPointData (self):
    """Returns start point"""
    return geomDataAPI_Point2D( self.my.data().attribute("StartPoint") )

  def endPointData (self):
    """Returns end point"""
    return geomDataAPI_Point2D( self.my.data().attribute("EndPoint") )

  def result (self):
    """Returns result"""
    return self.my.firstResult()


class Circle():
  """A class which represents a Circle object"""

  def __init__(self, sketch, x, y, r):
    """Constructor"""
    ### Create the feature
    self.my = sketch.addFeature("SketchCircle")
    geomDataAPI_Point2D( self.my.data().attribute("CircleCenter") ).setValue(x, y)
    self.my.data().real("CircleRadius").setValue(r)
    self.my.execute()

  def centerData (self):
    """Returns center point"""
    return geomDataAPI_Point2D( self.my.data().attribute("CircleCenter") )

  def result (self):
    """Returns result"""
    return self.my.lastResult()   # Returns the circular line attribute


