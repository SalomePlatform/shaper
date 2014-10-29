from ModelAPI import *

class PythonFeaturesPlugin_Box(ModelAPI_Feature):
  "Feature to create a box by drawing a sketch and extruding it"
  def __init__(self):
    pass

  def ID(self):
    return "Box"

  def WIDTH_ID(self):
    return "box_width"
  
  def LENGTH_ID(self):
    return "box_length"
  
  def HEIGHT_ID(self):
    return "box_height"

  def initAttributes(self):
    self.data().addAttribute(self.WIDTH_ID(), ModelAPI_AttributeDouble.type())
    self.data().addAttribute(self.LENGTH_ID(), ModelAPI_AttributeDouble.type())
    self.data().addAttribute(self.HEIGHT_ID(), ModelAPI_AttributeDouble.type())

  def execute(self):
    aWidth  = self.data().attribute(self.WIDTH_ID()).value()
    aLength = self.data().attribute(self.LENGTH_ID()).value()
    aHeight = self.data().attribute(self.HEIGHT_ID()).value()

    aResult = document().createBody(data())
    #aResult.store(UserPackage.makeBox(aLength, aWidth, aHeight)

    #self.setResult(aResult)


    


