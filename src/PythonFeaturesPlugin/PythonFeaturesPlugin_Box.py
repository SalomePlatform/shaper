import ModelAPI

from SketchResult import SketchResult
import extrusion
import sketch


class PythonFeaturesPlugin_Box(ModelAPI.ModelAPI_CompositeFeature):

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

    @staticmethod
    def WIDTH_REF_ID():
        return "box_ref_width"

    @staticmethod
    def LENGTH_REF_ID():
        return "box_ref_length"

    @staticmethod
    def HEIGHT_REF_ID():
        return "box_ref_height"

    def getKind(self):
        return PythonFeaturesPlugin_Box.ID()

    def initAttributes(self):
        # C++ static methods (in example "type()" of the ModelAPI_AttributeDouble
        # should be called like this: moduleName.ClassName_staticMethod()
        self.data().addAttribute(self.WIDTH_ID(), ModelAPI.ModelAPI_AttributeDouble_typeId())
        self.data().addAttribute(self.LENGTH_ID(), ModelAPI.ModelAPI_AttributeDouble_typeId())
        self.data().addAttribute(self.HEIGHT_ID(), ModelAPI.ModelAPI_AttributeDouble_typeId())
        self.data().addAttribute(self.WIDTH_REF_ID(), ModelAPI.ModelAPI_AttributeReference_typeId())
        self.data().addAttribute(self.LENGTH_REF_ID(), ModelAPI.ModelAPI_AttributeReference_typeId())
        self.data().addAttribute(self.HEIGHT_REF_ID(), ModelAPI.ModelAPI_AttributeReference_typeId())
        aSession = ModelAPI.ModelAPI_Session.get()
        aSession.validators().registerNotObligatory(self.getKind(), self.WIDTH_REF_ID())
        aSession.validators().registerNotObligatory(self.getKind(), self.LENGTH_REF_ID())
        aSession.validators().registerNotObligatory(self.getKind(), self.HEIGHT_REF_ID())
        aSession.validators().registerConcealment(self.getKind(), self.HEIGHT_REF_ID())
        self.mySketch = None # not yet initialized
        self.myExtrusion = None # not yet initialized

    def execute(self):
        aWidth = self.real(self.WIDTH_ID()).value()
        aLength = self.real(self.LENGTH_ID()).value()
        aHeight = self.real(self.HEIGHT_ID()).value()
        aWidthRefValue = self.reference(self.WIDTH_REF_ID()).value()
        aLengthRefValue = self.reference(self.LENGTH_REF_ID()).value()
        aHeightRefValue = self.reference(self.HEIGHT_REF_ID()).value()
        aResult = None
        if not all((aWidthRefValue, aLengthRefValue, aHeightRefValue)):
            aResult = extrusion.getBody(self.makeBox(aLength, aWidth, aHeight))
        else:
            aHeightProxyResult = ModelAPI.modelAPI_Result(aHeightRefValue)
            aWidthFeature = ModelAPI.modelAPI_Feature(aWidthRefValue)
            aLengthFeature = ModelAPI.modelAPI_Feature(aLengthRefValue)
            aHeightResult = ModelAPI.modelAPI_ResultBody(aHeightProxyResult)
            aWidthFeature.real("ConstraintValue").setValue(aWidth)
            aLengthFeature.real("ConstraintValue").setValue(aLength)
            if aHeightResult is not None:
                aHeightFeature = aHeightResult.document().feature(aHeightResult)
                aHeightFeature.real("extrusion_size").setValue(aHeight)
                aResult = extrusion.getBody(aHeightFeature)
        # create a new result with copied shape from extrusion
        aResultBody = self.document().createBody(self.data())
        if not aResult is None:
          aResultBody.store(aResult.shape())
          self.setResult(aResultBody)
        pass

    def makeBox(self, aWidth, aLength, aHeight):
        aSession = ModelAPI.ModelAPI_Session.get()
        aPart = aSession.activeDocument()
        # Starting the Sketch
        aSketch = sketch.addTo(aPart)
        self.mySketch = sketch
        sketch.setXOYPlane(aSketch)
        # Creating the lines
        l1 = sketch.addLine(10, 10, 10, 60, aSketch)
        l2 = sketch.addLine(10, 60, 60, 60, aSketch)
        l3 = sketch.addLine(60, 60, 60, 10, aSketch)
        l4 = sketch.addLine(60, 10, 10, 10, aSketch)
        aSketch.execute()
        # Creating the constraints
        sketch.makeCoincident(sketch.getEndPoint(l1), sketch.getStartPoint(l2), aSketch)
        sketch.makeCoincident(sketch.getEndPoint(l2), sketch.getStartPoint(l3), aSketch)
        sketch.makeCoincident(sketch.getEndPoint(l3), sketch.getStartPoint(l4), aSketch)
        sketch.makeCoincident(sketch.getEndPoint(l4), sketch.getStartPoint(l1), aSketch)
        sketch.makeParallel(sketch.getGeometry(l1), sketch.getGeometry(l3), aSketch)
        sketch.makeParallel(sketch.getGeometry(l2), sketch.getGeometry(l4), aSketch)
        sketch.makePerpendicular(sketch.getGeometry(l1), sketch.getGeometry(l4), aSketch)
        # Set to 0X and 0Y lines defined length
        aWidthFeature = sketch.makeConstantLength(sketch.getGeometry(l4), aWidth, aSketch)
        aLengthFeature = sketch.makeConstantLength(sketch.getGeometry(l1), aLength, aSketch)
        # Finalisation of the operation
        builder = SketchResult(aSketch)
        # Creating a feature Extrusion
        aHeightFeature = extrusion.addNew(builder, aHeight, aPart)
        self.myExtrusion = aHeightFeature
        # Store features...
        self.reference(self.WIDTH_REF_ID()).setValue(aWidthFeature)
        self.reference(self.LENGTH_REF_ID()).setValue(aLengthFeature)
        self.reference(self.HEIGHT_REF_ID()).setValue(aHeightFeature.firstResult())
        return aHeightFeature

    def addFeature(self, theID):
        pass

    def numberOfSubs(self):
        # extrusion and sketch
        return 2

    def subFeature(self, theIndex):
        if theIndex = 1: # sketch
          return self.mySketch
        return self.myExtrusion

    def subFeatureId(self, theIndex):
        return 0

    def isSub(self, theFeature):
        if theFeature = self.mySketch or theFeature = self.myExtrusion:
          return True
        return False

# TEST
"""
if __name__=='__main__':
  session = ModelAPI.ModelAPI_Session.get()
  part = session.activeDocument()
  session.startOperation()
  feature = part.addFeature('Box')
  feature.real('box_width').setValue(10)
  feature.real('box_length').setValue(10)
  feature.real('box_height').setValue(10)
  feature.execute()
  session.finishOperation()
"""
