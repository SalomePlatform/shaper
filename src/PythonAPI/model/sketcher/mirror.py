"""Sketch point feature interface."""

from GeomDataAPI import geomDataAPI_Point2D
from ModelAPI import ModelAPI_Feature
from model.roots import Interface
from model.errors import FeatureInputInvalid

class Mirror(Interface):
    """Interface on mirror constraint for data manipulation."""
    def __init__(self, feature, mirror_line, *mirror_objects):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "SketchConstraintMirror")
        
        self._feature.data().refattr("ConstraintEntityA").setObject(mirror_line)
        self._feature.data().reflist("ConstraintEntityB").clear()
        for object_ in mirror_objects:
            self._feature.data().reflist("ConstraintEntityB").append(object_)
            self._feature.data().reflist("ConstraintMirrorList").append(object_)
        self.execute()

    def mirroredObjects(self):
        return self._feature.data().reflist("ConstraintEntityC")
            #feature = ModelAPI_Feature(object_)
            #if feature.getKind() == "SketchCircle":
                #objects.append(Circle(feature))
            #elif feature.getKind() == "SketchLine":
                #objects.append(Line(feature))
            #else:
                #raise TypeError(
                    #"%s is not a valid feature type" % feature.getKind()
                    #)
                
