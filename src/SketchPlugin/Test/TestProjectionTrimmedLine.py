# Copyright (C) 2021-2026  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import os
from salome.kernel import salome
from salome.shaper import model

salome.salome_init()

# Use the 'data' subfolder to search for the BREP files
data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")

# Create part
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Import the "house" shape
Import_1 = model.addImport(Part_1_doc, os.path.join(data_dir, "house.brep"))
model.do()
Import_1.setName("House_1")
Import_1.result().setName("House_1_1")

# Create a Box and translate it
Box_1 = model.addBox(Part_1_doc, 15, 15, 63)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], vector = [-3.5, -3.5, 0], keepSubResults = True)
Translation_1.result().setTransparency(0.67)

# Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Translation_1_1/MF:Translated&Box_1_1/Left"))

# Project a "normal line" to the sketch
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[House_1_1/Shape_10][House_1_1/Shape_11]"), True)
SketchLine_1 = SketchProjection_1.createdFeature()
model.do()

# Project a "trimmed line" to the sketch
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[House_1_1/Shape_11][House_1_1/Shape_12]"), True)
SketchLine_2 = SketchProjection_2.createdFeature()
model.do()
model.end()

# Check the result
assert SketchLine_1 != None, "Normal line edge could not be projected"
assert SketchLine_1.getKind() == 'SketchLine', "Project line is of unexpected type"
assert SketchLine_2 != None, "Trimmed line edge could not be projected"
assert SketchLine_2 != None and SketchLine_2.getKind() == 'SketchLine', "Projected trimmed line is of unexpected type"
