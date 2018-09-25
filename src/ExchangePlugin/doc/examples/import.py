# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Import_1 = model.addImport(Part_1_doc, "/dn48/newgeom/for_gzy/gzy_data/Compound_2.brep")
model.do()
Import_1.setName("Compound_2")
Import_1.result().setName("Compound_2_1")
Import_1.result().subResult(0).setName("Compound_2_1_1")
Import_1.result().subResult(1).setName("Compound_2_1_2")
Import_1.result().subResult(2).setName("Compound_2_1_3")
Import_1.result().subResult(3).setName("Compound_2_1_4")
Import_1.result().subResult(4).setName("Compound_2_1_5")
model.do()
model.end()
