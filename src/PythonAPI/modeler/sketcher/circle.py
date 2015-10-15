from GeomDataAPI import geomDataAPI_Point2D

class Circle():

  def __init__(self, sketch, x, y, r):
    self.my = sketch.addFeature("SketchCircle")
    geomDataAPI_Point2D( self.my.data().attribute("CircleCenter") ).setValue(x, y)
    self.my.data().real("CircleRadius").setValue(r)
    self.my.execute()

  def centerData (self):
    return geomDataAPI_Point2D( self.my.data().attribute("CircleCenter") )

  def result (self):
    return self.my.lastResult()   # Returns the circular line attribute
