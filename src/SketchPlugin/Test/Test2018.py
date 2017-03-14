"""
    Test2018.py
    Test case for issue #2018 "After split auxiliary circle edit of axiliary state of arc is wrong"
"""

from ModelAPI import ModelAPI_Feature
from SketchAPI import SketchAPI_Arc
from salome.shaper import model

def getArcs(theSketch):
    anArcs = []
    for aSubObj in theSketch.features().list():
        aFeature = ModelAPI_Feature.feature(aSubObj)
        if aFeature is not None and aFeature.getKind() == "SketchArc":
            anArcs.append(aFeature)
    return anArcs


model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-316.2976090885591, -103.0319555403832, 230.3644881376851)
SketchCircle_1.setAuxiliary(True)
SketchLine_1 = Sketch_1.addLine(-489.6131019153831, 48.7229038254136, -457.9545454545454, 334.0909090909091)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchLine_2 = Sketch_1.addLine(-457.9545454545454, 334.0909090909091, -251.2069112222553, 117.9454153513332)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_1.results()[1])
Sketch_1.addSplit(SketchCircle_1, SketchLine_1.startPoint(), SketchLine_2.endPoint())
model.do()

anArcs = getArcs(Sketch_1)
# check auxiliary flags
for arc in anArcs:
    aCurrentArc = SketchAPI_Arc(arc)
    assert(aCurrentArc.auxiliary().value())
    aCurrentArc.setAuxiliary(False)
    model.do()
    assert(aCurrentArc.auxiliary().value() == False)

model.end()

assert(model.checkPythonDump())
