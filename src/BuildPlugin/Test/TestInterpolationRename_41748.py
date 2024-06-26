#!/usr/bin/env python

###
### This file is generated automatically by SALOME v9.12.0 with dump python functionality
###

# Test fix of bos #41748 [CEA] Issue in interpolation edition

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
model.addParameter(Part_1_doc, "r_canal", '0.00993')
model.addParameter(Part_1_doc, "r_helicoid", '0.109')
model.addParameter(Part_1_doc, "r_octagon", 'r_canal*0.5')
model.addParameter(Part_1_doc, "angle_octagon", '120')
model.addParameter(Part_1_doc, "deg_to_rad", '0.0174533')
model.addParameter(Part_1_doc, "spin_number", '2')
model.addParameter(Part_1_doc, "dz_by_spin", '0.0435')

### Create Interpolation
Interpolation_1 = model.addInterpolation(Part_1_doc, "r_helicoid*cos(deg_to_rad*t)", "r_helicoid*sin(deg_to_rad*t)", "dz_by_spin*t/360", 0, 720, 800)
Interpolation_1.setName("Helicoid_Path_Interpolation_Fct")
Interpolation_1.result().setName("Helicoid_Path_Interpolation")

print(Interpolation_1.xt().value())

print(Interpolation_1.xt().value())
Interpolation_1.xt().setValue("r_helicoid*cos(deg_to_rad*t)*1")
print(Interpolation_1.xt().value())

model.end()

model.checkFeaturesValidity(Part_1_doc)

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
