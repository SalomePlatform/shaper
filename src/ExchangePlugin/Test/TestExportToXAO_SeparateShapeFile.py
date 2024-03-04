# Copyright (C) 2018-2024  CEA, EDF
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

from salome.shaper import model

from GeomAPI import *

import tempfile
import os
import math

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Cyl_1 = model.addCylinder(Part_1_doc, 7, 13)
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box_1_1/Top")])

Field_1 = model.addField(Part_1_doc, 1, "STRING", 1, ["Comp 1"], [model.selection("SOLID", "Box_1_1")])
Field_1.addStep(0, 0, [[""], ["box"]])

with tempfile.TemporaryDirectory() as tmpdirname:
  # Export entire Part
  tmp_file1 = os.path.join(tmpdirname, "Part.xao")
  tmp_file2 = os.path.join(tmpdirname, "Part_shape.brep")

  Export_1 = model.exportToXAO(Part_1_doc, tmp_file1, "", "", tmp_file2)
  # due to the issue 2530, the export feature is removed after execution, so,
  # if export was performed correctly, the feature must become invalid
  #assert(Export_1.feature().error() == "")
  assert(not Export_1.feature().data().isValid())
  assert(os.path.isfile(tmp_file1))
  assert(os.path.isfile(tmp_file2))

  # Add two fields and one group to the Box_1
  Field_2 = model.addField(Part_1_doc, 1, "BOOLEAN", 1, ["Comp 1"], [model.selection("FACE", "Box_1_1/Front")])
  Field_2.addStep(0, 0, [[False], [True]])

  Field_3_objects = [model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]")]
  Field_3 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], Field_3_objects)
  Field_3.addStep(0, 0, [[0], [1], [0], [0], [0]])

  Group_2 = model.addGroup(Part_1_doc, "Edges", [model.selection("EDGE", "[Box_1_1/Top][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Top][Box_1_1/Right]")])

  tmp_file3 = os.path.join(tmpdirname, "Box.xao")
  tmp_file4 = os.path.join(tmpdirname, "Box_shape.brep")

  Export_2 = model.exportToXAO(Part_1_doc, tmp_file3, model.selection("SOLID", "Box_1_1"),
                               "", "", tmp_file4)
  assert(Export_2.feature().error() == "")
  assert(os.path.isfile(tmp_file3))
  assert(os.path.isfile(tmp_file4))

  model.end()

  # Try to import the exported data
  model.begin()
  partSet = model.moduleDocument()
  Part_1 = model.addPart(partSet)
  Part_1_doc = Part_1.document()

  Import_1 = model.addImport(Part_1_doc, tmp_file1)
  model.do()
  assert(Import_1.feature().error() == "")

  model.testNbResults(Import_1, 1)
  model.testNbSubResults(Import_1, [2])
  model.testNbSubShapes(Import_1, GeomAPI_Shape.SOLID, [2])
  model.testNbSubShapes(Import_1, GeomAPI_Shape.FACE, [9])
  model.testNbSubShapes(Import_1, GeomAPI_Shape.EDGE, [30])
  model.testNbSubShapes(Import_1, GeomAPI_Shape.VERTEX, [60])
  model.testResultsVolumes(Import_1, [3001.1945203])
  model.testResultsAreas(Import_1, [1479.645943])

  assert(Import_1.name() == "Part")
  assert(Import_1.result().name() == "Part_1")
  assert(Import_1.subFeature(0).name() == "Group_1")
  assert(Import_1.subFeature(1).name() == "Field_1")
  assert(Import_1.subFeature(0).result().name() == "Group_1")
  assert(Import_1.subFeature(1).result().name() == "Field_1")

  # check group
  assert(Part_1_doc.size("Groups") == 1)
  res1 = model.objectToResult(Part_1_doc.object("Groups", 0))
  assert(res1 is not None)
  res1It = GeomAPI_ShapeExplorer(res1.shape(), GeomAPI_Shape.FACE)
  assert(res1It.more())
  shape1 = res1It.current()
  res1It.next()
  assert(not res1It.more())
  p1 = res1.shape().middlePoint()
  aTol = 1.e-7
  assert(math.fabs(p1.x() - 5) <= aTol and math.fabs(p1.y() - 5) <= aTol and math.fabs(p1.z() - 10) <= aTol), "({}, {}, {}) != ({}, {}, {})".format(p1.x(), p1.y(), p1.z(), 5, 5, 10)

  Import_2 = model.addImport(Part_1_doc, tmp_file3)
  model.do()
  assert(Import_2.feature().error() == "")

  model.testNbResults(Import_2, 1)
  model.testNbSubResults(Import_2, [0])
  model.testNbSubShapes(Import_2, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Import_2, GeomAPI_Shape.FACE, [6])
  model.testNbSubShapes(Import_2, GeomAPI_Shape.EDGE, [24])
  model.testNbSubShapes(Import_2, GeomAPI_Shape.VERTEX, [48])
  model.testResultsVolumes(Import_2, [1000])
  model.testResultsAreas(Import_2, [600])

  assert(Import_2.name() == "Box_1_1")
  assert(Import_2.result().name() == "Box_1_1_1")
  assert(Import_2.subFeature(0).name() == "Group_1")
  assert(Import_2.subFeature(1).name() == "Group_2")
  assert(Import_2.subFeature(2).name() == "Field_1")
  assert(Import_2.subFeature(3).name() == "Field_2")
  assert(Import_2.subFeature(4).name() == "Field_3")
  assert(Import_2.subFeature(0).result().name() == "Group_1")
  assert(Import_2.subFeature(1).result().name() == "Group_2")
  assert(Import_2.subFeature(2).result().name() == "Field_1")
  assert(Import_2.subFeature(3).result().name() == "Field_2")
  assert(Import_2.subFeature(4).result().name() == "Field_3")

  # check groups
  assert(Part_1_doc.size("Groups") == 3)

  res1 = model.objectToResult(Part_1_doc.object("Groups", 1))
  assert(res1 is not None)
  res1It = GeomAPI_ShapeExplorer(res1.shape(), GeomAPI_Shape.FACE)
  assert(res1It.more())
  shape1 = res1It.current()
  res1It.next()
  assert(not res1It.more())
  p1 = res1.shape().middlePoint()
  aTol = 1.e-7
  assert(math.fabs(p1.x() - 5) <= aTol and math.fabs(p1.y() - 5) <= aTol and math.fabs(p1.z() - 10) <= aTol), "({}, {}, {}) != ({}, {}, {})".format(p1.x(), p1.y(), p1.z(), 5, 5, 10)

  res2 = model.objectToResult(Part_1_doc.object("Groups", 2))
  assert(res2 is not None)
  res2It = GeomAPI_ShapeExplorer(res2.shape(), GeomAPI_Shape.EDGE)
  assert(res2It.more())
  edge1 = res2It.current()
  res2It.next()
  edge2 = res2It.current()
  res2It.next()
  assert(not res2It.more())
  p2 = res2.shape().middlePoint()
  aTol = 1.e-7
  assert(math.fabs(p1.x() - 5) <= aTol and math.fabs(p1.y() - 5) <= aTol and math.fabs(p1.z() - 10) <= aTol), "({}, {}, {}) != ({}, {}, {})".format(p1.x(), p1.y(), p1.z(), 5, 5, 10)

  pass

model.end()
