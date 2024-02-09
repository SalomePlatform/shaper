#!/usr/bin/env python

###
### This file is generated automatically by SALOME v9.12.0 with dump python functionality
###

import sys
import salome

salome.salome_init()

###
### SHAPER component
###

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()

#########################################################
assert (len(Box_1.vertices()) == 48)
assert (len(Box_1.defaultResult().vertices()) == 48)
assert (len(Box_1.feature().vertices()) == 48)

# Count only unique vertices
assert (len(Box_1.vertices(True) ) == 8)
assert (len(Box_1.defaultResult().vertices(True)) == 8)
assert (len(Box_1.feature().vertices(True)) == 8)

#########################################################
assert (len(Box_1.edges()) == 24)
assert (len(Box_1.defaultResult().edges()) == 24)
assert (len(Box_1.feature().edges()) == 24)

# Count only unique edges
assert (len(Box_1.edges(True)) == 12)
assert (len(Box_1.defaultResult().edges(True)) == 12)
assert (len(Box_1.feature().edges(True)) == 12)

#########################################################
assert (len(Box_1.wires()) == 6)
assert (len(Box_1.defaultResult().wires()) == 6)
assert (len(Box_1.feature().wires()) == 6)

#########################################################
assert (len(Box_1.faces()) == 6)
assert (len(Box_1.defaultResult().faces()) == 6)
assert (len(Box_1.feature().faces()) == 6)

#########################################################
assert (len(Box_1.shells()) == 1)
assert (len(Box_1.defaultResult().shells()) == 1)
assert (len(Box_1.feature().shells()) == 1)

#########################################################
assert (len(Box_1.solids()) == 1)
assert (len(Box_1.defaultResult().solids()) == 1)
assert (len(Box_1.feature().solids()) == 1)


### Create Box
Box_2 = model.addBox(Part_1_doc, 40, 40, 5, 5, 5, 5)

### Create Box
Box_3 = model.addBox(Part_1_doc, 40, 40, 15, 5, 5, 5)

### Create Box
Box_4 = model.addBox(Part_1_doc, 40, 40, 25, 5, 5, 5)

### Create CompSolid
CompSolid_1_objects = [model.selection("SOLID", "Box_2_1"),
                       model.selection("SOLID", "Box_3_1"),
                       model.selection("SOLID", "Box_4_1")]
CompSolid_1 = model.addCompSolid(Part_1_doc, CompSolid_1_objects)
model.do()

#########################################################
assert (len(CompSolid_1.vertices()) == 144)
assert (len(CompSolid_1.defaultResult().vertices()) == 144)
assert (len(CompSolid_1.feature().vertices()) == 144)

# Count only unique vertices
assert (len(CompSolid_1.vertices(True) ) == 16)
assert (len(CompSolid_1.defaultResult().vertices(True)) == 16)
assert (len(CompSolid_1.feature().vertices(True)) == 16)

#########################################################
assert (len(CompSolid_1.edges()) == 72)
assert (len(CompSolid_1.defaultResult().edges()) == 72)
assert (len(CompSolid_1.feature().edges()) == 72)

# Count only unique edges
assert (len(CompSolid_1.edges(True)) == 28)
assert (len(CompSolid_1.defaultResult().edges(True)) == 28)
assert (len(CompSolid_1.feature().edges(True)) == 28)

#########################################################
assert (len(CompSolid_1.wires()) == 18)
assert (len(CompSolid_1.defaultResult().wires()) == 18)
assert (len(CompSolid_1.feature().wires()) == 18)

#########################################################
assert (len(CompSolid_1.faces()) == 18)
assert (len(CompSolid_1.defaultResult().faces()) == 18)
assert (len(CompSolid_1.feature().faces()) == 18)

#########################################################
assert (len(CompSolid_1.shells()) == 3)
assert (len(CompSolid_1.defaultResult().shells()) == 3)
assert (len(CompSolid_1.feature().shells()) == 3)

#########################################################
assert (len(CompSolid_1.solids()) == 3)
assert (len(CompSolid_1.defaultResult().solids()) == 3)
assert (len(CompSolid_1.feature().solids()) == 3)

#########################################################
### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(20, 20, 5)
model.do()
### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
model.do()

#########################################################
### Result is a single face (no shell or solid)
assert (len(Face_1.faces()) == 1)
assert (len(Face_1.shells()) == 0)
assert (len(Face_1.solids()) == 0)

#########################################################
### Create Common
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("COMPSOLID", "CompSolid_1_1")], keepSubResults = True)
model.do()

#########################################################
### There is no result created by the Common_1 feature
assert (Common_1.defaultResult() == None) # use this check to identify a non-existent result (feature can still ba valid!!)
assert (len(Common_1.faces()) == 0)
assert (len(Common_1.solids()) == 0)
assert (len(Common_1.feature().solids()) == 0)

model.end()
