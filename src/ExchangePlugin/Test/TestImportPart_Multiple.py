# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

from GeomAlgoAPI import *
from ModelAPI import *
from SketchAPI import *
from salome.shaper import model

import os
import math

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 100)
Translation_1.setName("MovedBox")
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), model.selection("EDGE", "PartSet/OY"), 45)
model.do()
model.end()

filename = 'check_export.shaperpart'
model.removeFile(filename)

# export all features from Part_1
model.begin()
model.exportPart(Part_1_doc, filename)
model.end()
assert os.path.exists(filename), "ERROR: Cannot export features from {}".format(Part_1.name())

# close all documents
model.reset()

# create new Part
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-20, -40, -50, -40)
SketchLine_2 = Sketch_1.addLine(-50, -40, -50, -10)
SketchLine_3 = Sketch_1.addLine(-50, -10, -20, -10)
SketchLine_4 = Sketch_1.addLine(-20, -10, -20, -40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 30)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_3.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 20)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_3.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 10)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 30, 0)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OX"), 50)
model.end()


def checkUniqueNames(theDocument):
    features = theDocument.allObjects()
    names = set()
    for feat in features:
        name = feat.data().name()
        if not name == "":
            assert(not name in names), "'{}' already exists in {}".format(name, names)
            names.add(name)


# import the document at the end
model.begin()
model.importPart(Part_1_doc, filename)
model.end()
checkUniqueNames(Part_1_doc)

# import the document after Extrusion_1
model.begin()
model.importPart(Part_1_doc, filename, Extrusion_1)
model.end()
checkUniqueNames(Part_1_doc)

# import the document after Sketch_1
model.begin()
model.importPart(Part_1_doc, filename, Sketch_1)
model.end()
checkUniqueNames(Part_1_doc)

# import the document after Translation_1
model.begin()
model.importPart(Part_1_doc, filename, Translation_1)
model.end()
checkUniqueNames(Part_1_doc)

assert(model.checkPythonDump())
