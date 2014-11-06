from ModelAPI import *

class PythonFeaturesPlugin_Box(ModelAPI_Feature):
  "Feature to create a box by drawing a sketch and extruding it"
  def __init__(self):
    pass

  @staticmethod
  def ID():
    return "Box"

  @staticmethod
  def WIDTH_ID():
    return "box_width"
  
  @staticmethod
  def LENGTH_ID():
    return "box_length"
  
  @staticmethod
  def HEIGHT_ID():
    return "box_height"

  def initAttributes(self):
    self.data().addAttribute(PythonFeaturesPlugin_Box.WIDTH_ID(), ModelAPI_AttributeDouble.type())
    self.data().addAttribute(PythonFeaturesPlugin_Box.LENGTH_ID(), ModelAPI_AttributeDouble.type())
    self.data().addAttribute(PythonFeaturesPlugin_Box.HEIGHT_ID(), ModelAPI_AttributeDouble.type())

  def execute(self):
    aWidth  = self.attribute(PythonFeaturesPlugin_Box.WIDTH_ID()).value()
    aLength = self.attribute(PythonFeaturesPlugin_Box.LENGTH_ID()).value()
    aHeight = self.attribute(PythonFeaturesPlugin_Box.HEIGHT_ID()).value()

    aResult = document().createBody(data())
    #aResult.store(UserPackage.makeBox(aLength, aWidth, aHeight)

    #self.setResult(aResult)


    


