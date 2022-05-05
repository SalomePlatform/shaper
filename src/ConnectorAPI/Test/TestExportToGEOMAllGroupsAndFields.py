# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
import tempfile

import sys
import salome

salome.standalone()
salome.salome_init()
theStudy = salome.myStudy

from salome.shaper import model

from salome.geom import geomBuilder
geompy = geomBuilder.New()

## Get the last object published in the GEOM section of the object browser
def getLastGEOMShape():
  sb = salome.myStudy.NewBuilder()
  comp = salome.myStudy.FindComponent("GEOM")
  obj = None
  if comp:
    iterator = salome.myStudy.NewChildIterator( comp )
    sobj = None
    while iterator.More():
      sobj = iterator.Value()
      iterator.Next()
    if sobj:
      obj = sobj.GetObject()
  else:
    raise Exception("GEOM component not found.")
  return obj

## Get the sub-object i of an object in the object browser
# Numerotation starts at 1
def getSubObject(obj, i):
  ok, sub_sobj = salome.ObjectToSObject(obj).FindSubObject(i)
  if not ok:
    raise Exception("No child found at %i for %s"%(i, obj.GetName()))
  sub_obj = sub_sobj.GetObject()
  return sub_obj

def dumpShaper(fileName):
  model.begin()
  dump=model.moduleDocument().addFeature("Dump")
  dump.string("file_path").setValue(fileName)
  dump.string("file_format").setValue("py")
  dump.boolean("topological_naming").setValue(True)
  dump.boolean("geometric_selection").setValue(False)
  dump.boolean("weak_naming").setValue(False)
  model.do()
  model.end()
  pass

def testGroupsAndFieldsExportToGEOM():
  model.begin()
  partSet = model.moduleDocument()
  Part_1 = model.addPart(partSet)
  Part_1_doc = Part_1.document()
  Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
  Box_2 = model.addBox(Part_1_doc, 20, 20, 20)
  Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), -10)
  Translation_1.result().setName("Translation_1_1")
  Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Translation_1_1"), model.selection("SOLID", "Box_2_1")])
  Partition_1.result().setName("Partition_1_1")
  Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
  Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OX"), 20)
  Translation_2.result().setName("Translation_2_1")
  Group_1_objects = [model.selection("FACE", "Translation_1_1/MF:Translated&Box_1_1/Top"), model.selection("FACE", "Box_2_1/Top"), model.selection("FACE", "Translation_2_1/MF:Translated&Box_3_1/Top")]
  Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
  Group_1.result().setName("faces_top")
  Group_2_objects = [model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Left][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_2_1/Left][Partition_1_1_2/Modified_Face&Box_2_1/Bottom]"), model.selection("EDGE", "[Translation_2_1/MF:Translated&Box_3_1/Left][Translation_2_1/MF:Translated&Box_3_1/Bottom]")]
  Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
  Group_2.result().setName("edges_x")
  Group_3_objects = [model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Left][Translation_2_1/MF:Translated&Box_3_1/Bottom]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Right][Translation_2_1/MF:Translated&Box_3_1/Bottom]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Back][Translation_2_1/MF:Translated&Box_3_1/Right][Translation_2_1/MF:Translated&Box_3_1/Bottom]"), model.selection("VERTEX", "[Box_2_1/Front][Box_2_1/Right][Partition_1_1_2/Modified_Face&Box_2_1/Bottom]"), model.selection("VERTEX", "[(Partition_1_1_2/Modified_Face&Box_2_1/Back&Box_1_1/Front)(Partition_1_1_2/Modified_Face&Box_2_1/Left)(Box_2_1/Right)(Partition_1_1_2/Modified_Face&Box_2_1/Bottom)(Box_2_1/Top)][Box_2_1/Right][Partition_1_1_2/Modified_Face&Box_2_1/Bottom]"), model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Box_2_1/Back&Box_1_1/Front][Translation_1_1/MF:Translated&Box_1_1/Right][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"), model.selection("VERTEX", "[Translation_1_1/MF:Translated&Box_1_1/Back][Translation_1_1/MF:Translated&Box_1_1/Right][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"), model.selection("VERTEX", "[Translation_1_1/MF:Translated&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Left][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]")]
  Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
  Group_3.result().setName("vertices_bottom")
  Group_4 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_1"), model.selection("SOLID", "Translation_2_1")])
  Group_4.result().setName("solids_small")
  Field_1_objects = [model.selection("SOLID", "Partition_1_1_1"), model.selection("SOLID", "Partition_1_1_2"), model.selection("SOLID", "Translation_2_1")]
  Field_1 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], Field_1_objects)
  Field_1.result().setName("Field_solids")
  Field_1.addStep(0, 0, [[0], [1], [2], [3]])
  Field_2_objects = [model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_2_1/Bottom"), model.selection("FACE", "Translation_2_1/MF:Translated&Box_3_1/Bottom"), model.selection("FACE", "Translation_1_1/MF:Translated&Box_1_1/Top"), model.selection("FACE", "Box_2_1/Top"), model.selection("FACE", "Translation_2_1/MF:Translated&Box_3_1/Top")]
  Field_2 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], Field_2_objects)
  Field_2.result().setName("Field_faces")
  Field_2.addStep(0, 0, [[0], [1], [1], [1], [2], [2], [2]])
  Field_3_objects = [model.selection("EDGE", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Left]"), model.selection("EDGE", "[Translation_2_1/MF:Translated&Box_3_1/Left][Translation_2_1/MF:Translated&Box_3_1/Bottom]"), model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_2_1/Left][Partition_1_1_2/Modified_Face&Box_2_1/Bottom]"), model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Left][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[Translation_1_1/MF:Translated&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Left]"), model.selection("EDGE", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Right]"), model.selection("EDGE", "[Translation_1_1/MF:Translated&Box_1_1/Back][Translation_1_1/MF:Translated&Box_1_1/Right]"), model.selection("EDGE", "[Box_2_1/Front][Box_2_1/Right]"), model.selection("EDGE", "[Box_2_1/Right][Box_2_1/Top]"), model.selection("EDGE", "[(Partition_1_1_2/Modified_Face&Box_2_1/Back&Box_1_1/Front)(Partition_1_1_2/Modified_Face&Box_2_1/Left)(Box_2_1/Right)(Partition_1_1_2/Modified_Face&Box_2_1/Bottom)(Box_2_1/Top)][Box_2_1/Right]"), model.selection("EDGE", "[Box_2_1/Right][Partition_1_1_2/Modified_Face&Box_2_1/Bottom]")]
  Field_3 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], Field_3_objects)
  Field_3.result().setName("Field_edges")
  Field_3.addStep(0, 0, [[0], [1], [1], [1], [2], [2], [2], [2], [3], [3], [3], [3]])
  Field_4_objects = [model.selection("VERTEX", "[Translation_1_1/MF:Translated&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Left][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"), model.selection("VERTEX", "[Translation_1_1/MF:Translated&Box_1_1/Back][Translation_1_1/MF:Translated&Box_1_1/Right][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"), model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Box_2_1/Back&Box_1_1/Front][Translation_1_1/MF:Translated&Box_1_1/Right][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"), model.selection("VERTEX", "[(Partition_1_1_2/Modified_Face&Box_2_1/Back&Box_1_1/Front)(Partition_1_1_2/Modified_Face&Box_2_1/Left)(Box_2_1/Right)(Partition_1_1_2/Modified_Face&Box_2_1/Bottom)(Box_2_1/Top)][Box_2_1/Right][Partition_1_1_2/Modified_Face&Box_2_1/Bottom]"), model.selection("VERTEX", "[Box_2_1/Front][Box_2_1/Right][Partition_1_1_2/Modified_Face&Box_2_1/Bottom]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Back][Translation_2_1/MF:Translated&Box_3_1/Right][Translation_2_1/MF:Translated&Box_3_1/Bottom]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Right][Translation_2_1/MF:Translated&Box_3_1/Bottom]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Left][Translation_2_1/MF:Translated&Box_3_1/Bottom]"), model.selection("VERTEX", "[Translation_1_1/MF:Translated&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Left][Translation_1_1/MF:Translated&Box_1_1/Top]"), model.selection("VERTEX", "[Translation_1_1/MF:Translated&Box_1_1/Back][Translation_1_1/MF:Translated&Box_1_1/Right][Translation_1_1/MF:Translated&Box_1_1/Top]"), model.selection("VERTEX", "[(Partition_1_1_2/Modified_Face&Box_2_1/Back&Box_1_1/Front)(Partition_1_1_2/Modified_Face&Box_2_1/Left)(Box_2_1/Right)(Partition_1_1_2/Modified_Face&Box_2_1/Bottom)(Box_2_1/Top)][Partition_1_1_2/Modified_Face&Box_2_1/Left][Box_2_1/Top]"), model.selection("VERTEX", "[(Partition_1_1_2/Modified_Face&Box_2_1/Back&Box_1_1/Front)(Partition_1_1_2/Modified_Face&Box_2_1/Left)(Box_2_1/Right)(Partition_1_1_2/Modified_Face&Box_2_1/Bottom)(Box_2_1/Top)][Box_2_1/Right][Box_2_1/Top]"), model.selection("VERTEX", "[Box_2_1/Front][Partition_1_1_2/Modified_Face&Box_2_1/Left][Box_2_1/Top]"), model.selection("VERTEX", "[Box_2_1/Front][Box_2_1/Right][Box_2_1/Top]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Left][Translation_2_1/MF:Translated&Box_3_1/Top]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Right][Translation_2_1/MF:Translated&Box_3_1/Top]")]
  Field_4 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], Field_4_objects)
  Field_4.result().setName("Field_vertices")
  Field_4.addStep(0, 0, [[0], [1], [1], [1], [1], [1], [1], [1], [1], [2], [2], [2], [2], [3], [3], [3], [3]])
  model.do()
  model.exportToGEOM(Part_1_doc)
  model.end()

# check the groups content by the coordinates of a point on its sub-shapes
def checkGroupByCoords(group, coords, tolerance=1e-7):
  for coord in coords:
    p = geompy.MakeVertex(*coord)
    assert geompy.MinDistance(group, p) < tolerance
  pass

## Check the result imported in GEOM
def checkResultInGEOM():
  geomObject_1 = getLastGEOMShape() # the second, translation result

  group_1_GEOM = getSubObject(geomObject_1, 1)
  assert group_1_GEOM.GetName() == 'faces_top'
  assert geompy.NumberOfFaces(group_1_GEOM) == 1 # 2 faces are in the first result, only one here

  # coordinates of the barycenters of the faces of Group_1
  coords_1 = [[25, 5, 10]]
  checkGroupByCoords(group_1_GEOM, coords_1)

  group_2_GEOM = getSubObject(geomObject_1, 2)
  assert group_2_GEOM.GetName() == 'edges_x'
  assert geompy.NumberOfEdges(group_2_GEOM) == 1 # 2 edges are in the first result, only one here

  # coordinates of the barycenters of the edges of Group_2
  coords_2 = [[25, 0, 0]]
  checkGroupByCoords(group_2_GEOM, coords_2)

  group_3_GEOM = getSubObject(geomObject_1, 3)
  assert group_3_GEOM.GetName() == 'vertices_bottom'
  assert geompy.NumberOfSubShapes(group_3_GEOM, geompy.ShapeType["VERTEX"]) == 3 # 3 of 8

  # coordinates of the points of of Group_3
  coords_3 = [[20, 10, 0], [30, 10, 0], [30, 0, 0]]
  checkGroupByCoords(group_3_GEOM, coords_3)

  group_4_GEOM = getSubObject(geomObject_1, 4)
  assert group_4_GEOM.GetName() == 'solids_small'
  assert geompy.NumberOfSolids(group_4_GEOM) == 1 # 1 of 2

  # coordinates of the barycenters of the solids of Group_4
  coords_4 = [[25, 5, 5]]
  checkGroupByCoords(group_4_GEOM, coords_4)

  field_1_GEOM = getSubObject(geomObject_1, 5)
  assert field_1_GEOM.GetName() == 'Field_solids'
  assert field_1_GEOM.GetStep(1).GetValues() == [3.0]

  field_2_GEOM = getSubObject(geomObject_1, 6)
  assert field_2_GEOM.GetName() == 'Field_faces'
  assert field_2_GEOM.GetStep(1).GetValues() == [0.0, 0.0, 0.0, 0.0, 1.0, 2.0]

  field_3_GEOM = getSubObject(geomObject_1, 7)
  assert field_3_GEOM.GetName() == 'Field_edges'
  assert field_3_GEOM.GetStep(1).GetValues() == [0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 2.0, 0.0, 1.0, 0.0, 0.0, 0.0]

  field_4_GEOM = getSubObject(geomObject_1, 8)
  assert field_4_GEOM.GetName() == 'Field_vertices'
  assert field_4_GEOM.GetStep(1).GetValues() == [0.0, 0.0, 0.0, 1.0, 3.0, 1.0, 3.0, 1.0]

  pass

def checkDump():
  # Dump the salome study (only CORBA modules, SHAPER dump is not in it)
  with tempfile.TemporaryDirectory() as tempdir:
    dumpFileGeomBase = "dump_test_geom"
    dumpFileGeom = os.path.join(tempdir, "%s.py"%dumpFileGeomBase)
    salome.myStudy.DumpStudy(tempdir, dumpFileGeomBase, True, False)

    # Dump SHAPER
    dumpFileShaper = os.path.join(tempdir, "dump_test_shaper.py")
    dumpShaper(dumpFileShaper)

    # Load SHAPER dump
    exec(compile(open(dumpFileShaper).read(), dumpFileShaper, 'exec'))

    # Load GEOM dump
    exec(compile(open(dumpFileGeom).read(), dumpFileGeom, 'exec'))

    # Clean files
    files = [dumpFileGeom, dumpFileShaper]
  pass

if __name__ == '__main__':
  # create 3 boxes with groups and fields
  testGroupsAndFieldsExportToGEOM()
  # check the result in GEOM
  checkResultInGEOM()
  # check that dump produces no error and can be reloaded
  checkDump()
  # check the result of the dump
  checkResultInGEOM()
