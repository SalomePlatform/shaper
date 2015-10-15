from GeomDataAPI import geomDataAPI_Point2D


class Line():

  def __init__(self, sketch, *args):
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
    geomDataAPI_Point2D( self.my.data().attribute("StartPoint") ).setValue(x1, y1)
    geomDataAPI_Point2D( self.my.data().attribute("EndPoint") ).setValue(x2, y2)
    self.my.execute()

  def __createByPoints(self, p1, p2):
    geomDataAPI_Point2D( self.my.data().attribute("StartPoint") ).setValue(p1.x(), p1.y())
    geomDataAPI_Point2D( self.my.data().attribute("EndPoint") ).setValue(p2.x(), p2.y())
    self.my.execute()

  def __createByName(self, sketch, name):
    self.my.data().selection("External").selectSubShape("EDGE", name)
    self.my.execute()
    rigid = sketch.addFeature("SketchConstraintRigid")
    rigid.refattr("ConstraintEntityA").setObject( self.my.firstResult() )

  def startPointData (self):
    return geomDataAPI_Point2D( self.my.data().attribute("StartPoint") )

  def endPointData (self):
    return geomDataAPI_Point2D( self.my.data().attribute("EndPoint") )

  def result (self):
    return self.my.firstResult()