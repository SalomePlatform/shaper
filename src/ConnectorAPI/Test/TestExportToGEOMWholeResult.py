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
from salome.shaper import model

from salome.geom import geomBuilder

import os
import tempfile

salome.salome_init(1)
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

# Create 2 boxes
# Create a group of all edges of the first box (whole result)
# Create a group of all faces of the whole partition result (whole result)
# exportToGEOM
# Check the result
# Check the dump
def testExportToGEOM():

  model.begin()
  partSet = model.moduleDocument()
  Part_1 = model.addPart(partSet)
  Part_1_doc = Part_1.document()
  Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
  Box_2 = model.addBox(Part_1_doc, 20, 20, 20)
  Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), -10)
  Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Translation_1_1"), model.selection("SOLID", "Box_2_1")])
  Group_1 = model.addGroup(Part_1_doc, "EDGE", [model.selection("COMPSOLID", "Partition_1_1_1")])
  Group_2 = model.addGroup(Part_1_doc, "FACE", [model.selection("COMPSOLID", "Partition_1_1")])
  model.do()

  model.exportToGEOM(Part_1_doc)
  model.end()

  # Check that the GEOM object has 1 compsolid and 2 solids
  geomObject_1 = getLastGEOMShape()
  assert geompy.NumberOfSubShapes(geomObject_1, geompy.ShapeType["COMPSOLID"]) == 1
  assert geompy.NumberOfSolids(geomObject_1) == 2

  # Check that the group has 12 edges
  geomGroup_1 = getSubObject(geomObject_1, 1)
  assert geompy.NumberOfEdges(geomGroup_1) == 12

  # Check that the group has 6+6 faces
  geomGroup_2 = getSubObject(geomObject_1, 2)
  assert geompy.NumberOfFaces(geomGroup_2) == 12

  # Dump the salome study (only CORBA modules, SHAPER dump is not in it)
  tempdir = tempfile.gettempdir()
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
  for f in files:
    os.remove(f)

  pass

if __name__ == '__main__':
  testExportToGEOM()
