from salome.shaper import model
import os

stlDataDir = os.environ.get('SALOME_SRC_DIR')+ "/../SHAPER/test.squish/shared/testdata/exchange/stl-examples"

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Import_1 = model.addImport(Part_1_doc, stlDataDir+"/cubeCylinder_BIN.stl")
Import_2 = model.addImport(Part_1_doc, stlDataDir+"/cubeCylinder_ASCII.stl")
model.do()
Import_1.setName("Compound_1")
Import_1.result().setName("Compound_1_1")
Import_1.result().subResult(0).setName("Compound_1_1_1")
Import_1.result().subResult(1).setName("Compound_1_1_2")
Import_2.setName("Compound_2")
Import_2.result().setName("Compound_2_1")
Import_2.result().subResult(0).setName("Compound_2_1_1")
Import_2.result().subResult(1).setName("Compound_2_1_2")
model.do()
model.end()
