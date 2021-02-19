from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Interpolation_1 = model.addInterpolation(Part_1_doc, "5*sin(t)","5*cos(t)","t*0.2", 0, 25, 100)
model.do()
model.end()
