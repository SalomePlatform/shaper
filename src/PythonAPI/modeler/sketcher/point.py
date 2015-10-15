# Class definitions of Sketch features

from GeomDataAPI import geomDataAPI_Point2D

class Point():

  def __init__(self, sketch, x, y):
    self.my = sketch.addFeature("SketchPoint")
    geomDataAPI_Point2D( self.my.data().attribute("PointCoordindates") ).setValue(x, y)
    self.my.execute()

  def pointData (self):
    return geomDataAPI_Point2D( self.my.data().attribute("PointCoordindates") )

  def result (self):
    return self.my.firstResult()
