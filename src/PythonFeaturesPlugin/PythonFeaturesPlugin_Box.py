import ModelAPI

class PythonFeaturesPlugin_Box(ModelAPI.ModelAPI_Feature):
  "Feature to create a box by drawing a sketch and extruding it"
  def __init__(self):
    ModelAPI.ModelAPI_Feature.__init__(self)

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

  def getKind(self):
    return PythonFeaturesPlugin_Box.ID()

  def initAttributes(self):
    # C++ static methods (in example "type()" of the ModelAPI_AttributeDouble
    # should be called like this: moduleName.ClassName_staticMethod()
    self.data().addAttribute(PythonFeaturesPlugin_Box.WIDTH_ID(), ModelAPI.ModelAPI_AttributeDouble_type())
    self.data().addAttribute(PythonFeaturesPlugin_Box.LENGTH_ID(), ModelAPI.ModelAPI_AttributeDouble_type())
    self.data().addAttribute(PythonFeaturesPlugin_Box.HEIGHT_ID(), ModelAPI.ModelAPI_AttributeDouble_type())

  def execute(self):
    aWidth  = self.real(PythonFeaturesPlugin_Box.WIDTH_ID()).value()
    aLength = self.real(PythonFeaturesPlugin_Box.LENGTH_ID()).value()
    aHeight = self.real(PythonFeaturesPlugin_Box.HEIGHT_ID()).value()
    # print ("Box W:{0} L:{1} H:{2}".format(aWidth, aLength, aHeight))
    # aResult = document().createBody(data())
    # aResult.store(UserPackage.makeBox(aLength, aWidth, aHeight)
    # self.setResult(aResult)


    


