from salome.shaper import model
from GeomAPI import *

#==============================================================================
# Auxiliary functions
#==============================================================================

# Project all features from 'theProjected' list.
# Argument 'theFailed' shows indices of projections which should fail because of validator
def testProjections(theDocument, theSketch, theProjected, theFailed):
    # generate list of projected features
    ind = 0
    edgeProj = set()
    for type, name in theProjected:
        proj = theSketch.addProjection(model.selection(type, name), True)
        assert(bool(proj.feature().error() != '') == bool(ind in theFailed))
        if proj.feature().error() != '':
            if proj.createdFeature() is not None:
                theDocument.removeFeature(proj.createdFeature().feature())
            theDocument.removeFeature(proj.feature())
            model.do()
        elif type == "EDGE":
            edgeProj.add(proj)
        ind += 1
    model.do()
    model.testNbSubShapes(theSketch, GeomAPI_Shape.EDGE, [len(edgeProj)])

    # exclude some edges from result
    NB_TO_EXCLUDE = 2
    num = 0
    for proj in edgeProj:
        proj.setIncludeToResult(False)
        num += 1
        if num >= NB_TO_EXCLUDE:
            break
    model.do()
    model.testNbSubShapes(theSketch, GeomAPI_Shape.EDGE, [len(edgeProj) - num])


#==============================================================================
# Initial model
#==============================================================================
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-187.9303398287678, -363.2915373234726, 182.2190183849013)
SketchArc_1 = Sketch_1.addArc(-229.9631763073051, -65.7360230979784, -105.4201011859997, -97.06956797196608, -326.3502666769664, 19.13032715412109, False)
SketchLine_1 = Sketch_1.addLine(-438.4308780225287, -71.00741660505224, 161.0737545348623, -582.1237015141244)
SketchPoint_1 = Sketch_1.addPoint(-446.0706301668712, -312.4620987423343)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_2 = Sketch_2.addLine(27.19276215608871, 61.51157581079401, 72.96621462024476, 0)
SketchLine_3 = Sketch_2.addLine(model.selection("EDGE", "PartSet/OZ"))
SketchLine_4 = Sketch_2.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_4.result())
SketchLine_5 = Sketch_2.addLine(72.96621462024476, 0, 0, 0)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_2.addLine(0, 0, 0, 61.51157581079401)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_3.result())
SketchLine_7 = Sketch_2.addLine(0, 61.51157581079401, 27.19276215608871, 61.51157581079401)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchLine_7.endPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_7.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_2r-SketchLine_5r-SketchLine_6r-SketchLine_7r")], model.selection(), 100, 0)
model.do()

#==============================================================================
# Tests
#==============================================================================

aProjectedList = [("EDGE", "Sketch_1/Edge-SketchCircle_1_2"),
                  ("EDGE", "Sketch_1/Edge-SketchArc_1_2"),
                  ("EDGE", "Sketch_1/Edge-SketchLine_1"),
                  ("VERTEX", "Sketch_1/Vertex-SketchPoint_1"),
                  #
                  ("VERTEX", "Sketch_1/Vertex-SketchCircle_1"),
                  ("VERTEX", "Sketch_1/Vertex-SketchLine_1e"),
                  ("VERTEX", "Sketch_1/Vertex-SketchLine_1s"),
                  ("VERTEX", "Sketch_1/Vertex-SketchArc_1_2s"),
                  ("VERTEX", "Sketch_1/Vertex-SketchArc_1_2e"),
                  ("VERTEX", "Sketch_1/Vertex-SketchArc_1"),
                  #
                  ("VERTEX", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"),
                  ("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/From_Face_1"),
                  ("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3&Extrusion_1_1/From_Face_1"),
                  ("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"),
                  ("VERTEX", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1"),
                  ("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1"),
                  ("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1"),
                  ("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1"),
                  #
                  ("EDGE", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/From_Face_1"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/From_Face_1"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1"),
                  #
                  ("EDGE", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_3"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_1&Extrusion_1_1/Generated_Face_2"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3"),
                  #
                  ("EDGE", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/To_Face_1"),
                  ("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/From_Face_1")
                  ]

# Test projection to the same plane
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_2"))
aFailedIDs = set([21, 29])
testProjections(Part_1_doc, Sketch_3, aProjectedList, aFailedIDs)

# Test projection to parallel plane
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_4"))
testProjections(Part_1_doc, Sketch_4, aProjectedList, aFailedIDs)

# Test projection to lower base of the prism
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/From_Face_1"))
aFailedIDs = set([0, 1, 22, 23, 24, 25])
testProjections(Part_1_doc, Sketch_5, aProjectedList, aFailedIDs)

# Test projection to upper base of the prism
Sketch_6 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face_1"))
testProjections(Part_1_doc, Sketch_6, aProjectedList, aFailedIDs)

# Test projection to orthogonal side face of the prism
Sketch_7 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_3"))
aFailedIDs = set([0, 1, 18, 20, 26, 28])
testProjections(Part_1_doc, Sketch_7, aProjectedList, aFailedIDs)

# Test projection to slope side face of the prism
Sketch_8 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_1"))
aFailedIDs = set([0, 1])
testProjections(Part_1_doc, Sketch_8, aProjectedList, aFailedIDs)

model.end()
