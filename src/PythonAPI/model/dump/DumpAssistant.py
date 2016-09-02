"""Package for dumping purposes.
"""

import ModelHighAPI

import ModelAPI
import SketchAPI

import sys
import inspect

def singleton(cls):
    instance = cls()
    instance.__call__ = lambda: instance
    return instance


## @ingroup CPPHighAPI
#  Collect information about features that may be dumped and stores the model as a Python script
@singleton
class DumpAssistant(ModelHighAPI.ModelHighAPI_Dumper):

    ## Constructor
    def __init__(self):
        ModelHighAPI.ModelHighAPI_Dumper.__init__(self)
        ModelHighAPI.ModelHighAPI_Dumper.setInstance(self)
        self.collectFeatures()

    ## Collect feature wrappers, which allow dumping (have method dump)
    def collectFeatures(self):
        self.myFeatures = {}
        self.myWrapperNames = {}
        for aModule in sys.modules:
            for aName, anObj in inspect.getmembers(sys.modules[aModule], inspect.isclass):
                if issubclass(anObj, ModelHighAPI.ModelHighAPI_Interface) and hasattr(anObj, "ID") and anObj.dump != ModelHighAPI.ModelHighAPI_Interface.dump:
                    self.myFeatures[anObj.ID()] = anObj
                    self.myWrapperNames[anObj.ID()] = aName

    ## Create wrapper for a given feature and dump it
    def dumpFeature(self, theFeature, theForce):
        aFeatureKind = theFeature.getKind()
        if aFeatureKind in self.myFeatures:
            # Dump only feature created by user (in history).
            # For all other features, just keep their name.
            if theForce or theFeature.isInHistory():
                self.myFeatures[aFeatureKind](theFeature).dump(self)
            else:
                self.name(theFeature)
                self.clearNotDumped()
        else:
            # Probably the feature is a constraint, try to dump it with SketchAPI_Constraint.
            # In case of theFeature is not a constraint, it will not be dumped.
            self.myFeatures[SketchAPI.SketchAPI_Constraint.ID()](theFeature).dump(self)

    ## Dump all parameters
    def dumpParameter(self, theFeature):
        aFeatureKind = theFeature.getKind()
        if aFeatureKind == "Parameter" and aFeatureKind in self.myFeatures:
            self.myFeatures[aFeatureKind](theFeature).dump(self)

    ## Return getter for specified attribute
    def attributeGetter(self, theFeature, theAttrName):
        aFeatureKind = theFeature.getKind()
        if aFeatureKind in self.myFeatures:
            return self.myFeatures[aFeatureKind](theFeature).attributeGetter(theAttrName)
        return std_string()

    ## Return name of wrapper feature
    def featureWrapper(self, theFeature):
        aFeatureKind = theFeature.getKind()
        if aFeatureKind in self.myWrapperNames:
            return self.myWrapperNames[aFeatureKind]
        return std_string()

# Instance of dumper
dumper = DumpAssistant