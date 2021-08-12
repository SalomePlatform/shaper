# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

import salome
from ModelAPI import *
from salome.shaper import model


import os
import tempfile

salome.standalone()
from salome.geom import geomBuilder
salome.salome_init(1)
geompy = geomBuilder.New()

## Get the last object published in the GEOM section of the object browser
def getGEOMShape(index):
  sb = salome.myStudy.NewBuilder()
  comp = salome.myStudy.FindComponent("GEOM")
  obj = None
  if comp:
    iterator = salome.myStudy.NewChildIterator( comp )
    sobj = None
    i = index + 1
    while iterator.More() and i:
      sobj = iterator.Value()
      iterator.Next()
      i = i - 1
    if i == 0 and sobj:
      obj = sobj.GetObject()
  else:
    raise Exception("GEOM component " + str(index) + " not found.")
  return obj

# create 4 parts:
#  just an extrusiob
#  extrusion with faces-group
#  polyline with vertices-field
#  part of one construction point

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(7, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 9)
SketchLine_3 = Sketch_1.addLine(0, 9, 7, 9)
SketchLine_4 = Sketch_1.addLine(7, 9, 7, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 7)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), 9)
SketchCircle_1 = Sketch_1.addCircle(3, 5, 2)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchCircle_1.center(), SketchLine_1.startPoint(), 5)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchCircle_1.center(), SketchLine_1.endPoint(), 3)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 2)
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r-SketchCircle_1_2r")], model.selection(), 10, 0)
model.do()
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Extrusion_2 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_2_doc, [model.selection("FACE", "Extrusion_1_1/Generated_Face&PartSet/Sketch_1/SketchCircle_1_2"), model.selection("FACE", "Extrusion_1_1/To_Face")])
model.do()
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Polyline_1_objects = [model.selection("VERTEX", "PartSet/Sketch_1/SketchLine_3_StartVertex"), model.selection("VERTEX", "PartSet/Sketch_1/SketchLine_4_EndVertex"), model.selection("VERTEX", "PartSet/Sketch_1/SketchLine_3_EndVertex")]
Polyline_1 = model.addPolyline3D(Part_3_doc, Polyline_1_objects, True)
Field_1_objects = [model.selection("VERTEX", "[Polyline_1_1/Generated_Edge&PartSet/Sketch_1/SketchLine_3_StartVertex]e[Polyline_1_1/Generated_Edge&PartSet/Sketch_1/SketchLine_4_EndVertex]e"), model.selection("VERTEX", "[Polyline_1_1/Generated_Edge&PartSet/Sketch_1/SketchLine_4_EndVertex]e[Polyline_1_1/Generated_Edge&PartSet/Sketch_1/SketchLine_3_EndVertex]e"), model.selection("VERTEX", "[Polyline_1_1/Generated_Edge&PartSet/Sketch_1/SketchLine_3_StartVertex]e[Polyline_1_1/Generated_Edge&PartSet/Sketch_1/SketchLine_3_EndVertex]e")]
Field_1 = model.addField(Part_3_doc, 2, "DOUBLE", 1, ["Comp 1"], Field_1_objects)
Field_1.addStep(0, 0, [[1.5], [1.1], [2.4], [3]])
Field_1.addStep(1, 2, [[1], [7], [5], [1.9]])
model.do()
Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()
Point_2 = model.addPoint(Part_4_doc, model.selection("EDGE", "PartSet/Sketch_1/SketchCircle_1_2"))
model.end()

# Activate Partset to export to geom all Parts with group and filed where it is needed
model.begin()
aSession = ModelAPI_Session.get()
partSet = aSession.moduleDocument()
aSession.setActiveDocument(partSet, True)
model.end()

# export to GEOM
model.begin()
model.exportToGEOM(Part_1_doc)
model.end()

# check that in GEOM module there are results:
# extrusion of the first part
# extrusion with group of the second part
# extrusion with filed of the third part
shape1 = getGEOMShape(0)
assert(shape1)
assert(shape1.GetName() == "Part_1")
assert(geompy.NumberOfSolids(shape1) == 1)
assert(not salome.ObjectToSObject(shape1).FindSubObject(1)[1])

shape2 = getGEOMShape(1)
assert(shape2)
assert(shape2.GetName() == "Part_2")
assert(geompy.NumberOfSolids(shape2) == 1)
group = salome.ObjectToSObject(shape2).FindSubObject(1)[1].GetObject()
assert(group)
assert(group.GetName() == "Group_1")
assert(geompy.NumberOfFaces(group) == 2)

shape3 = getGEOMShape(2)
assert(shape3)
assert(shape3.GetName() == "Part_3")
assert(geompy.NumberOfSolids(shape3) == 0)
assert(geompy.NumberOfSubShapes(shape3, geompy.ShapeType["VERTEX"]) == 3)
field = salome.ObjectToSObject(shape3).FindSubObject(1)[1].GetObject()
assert(field)
assert(field.GetName() == "Field_1")
assert(field.GetSteps() == [1, 2])

shape4 = getGEOMShape(3)
assert(not shape4)
