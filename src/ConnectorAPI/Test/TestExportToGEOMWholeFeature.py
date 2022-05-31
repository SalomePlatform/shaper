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
import salome

from SketchAPI import *
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

## Get the sub-object i of an object in the object browser
# Numerotation starts at 1
def getSubObject(obj, i):
  ok, sub_sobj = salome.ObjectToSObject(obj).FindSubObject(i)
  if not ok:
    raise Exception("No child found at %i for %s"%(i, obj.GetName()))
  sub_obj = sub_sobj.GetObject()
  return sub_obj


def testExportToGEOM():
  model.begin()
  partSet = model.moduleDocument()
  Part_1 = model.addPart(partSet)
  Part_1_doc = Part_1.document()
  Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
  SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
  SketchPoint_1 = SketchProjection_1.createdFeature()
  SketchCircle_1 = Sketch_1.addCircle(0, 0, 25)
  SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
  SketchCircle_2 = Sketch_1.addCircle(20, -15, 20)
  SketchCircle_3 = Sketch_1.addCircle(0, 25, 24.7213595499958)
  SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchCircle_1.results()[1], SketchCircle_3.center())
  SketchCircle_4 = Sketch_1.addCircle(-20, -15, 20)
  SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchCircle_1.results()[1], SketchCircle_4.center())
  SketchConstraintTangent_1 = Sketch_1.setTangent(SketchCircle_4.results()[1], SketchCircle_2.results()[1])
  SketchConstraintTangent_2 = Sketch_1.setTangent(SketchCircle_3.results()[1], SketchCircle_2.results()[1])
  SketchConstraintTangent_3 = Sketch_1.setTangent(SketchCircle_4.results()[1], SketchCircle_3.results()[1])
  SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchCircle_1.results()[1])
  SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_2.results()[1], SketchCircle_4.results()[1])
  SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_2.results()[1], 20)
  SketchConstraintRadius_2 = Sketch_1.setRadius(SketchCircle_1.results()[1], 25)
  SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
  SketchLine_1 = SketchProjection_2.createdFeature()
  SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_3.center())
  model.do()
  Extrusion_1_objects = [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_4_2r-SketchCircle_4_2r-SketchCircle_3_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_3_2r-SketchCircle_2_2r-SketchCircle_2_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_2_2r-SketchCircle_4_2r")]
  Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection(), 10, 0)
  # select all results of an extrusion feature
  Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("COMPSOLID", "all-in-Extrusion_1")])
  Group_2 = model.addGroup(Part_1_doc, "Faces", [model.selection("SOLID", "Extrusion_1_2")])
  GroupSubstraction_1 = model.addGroupSubstraction(Part_1_doc, [model.selection("COMPOUND", "Group_1")], [model.selection("COMPOUND", "Group_2")])
  model.do()
  model.exportToGEOM(Part_1_doc)
  model.end()

  # check that in GEOM module there are 3-SOLIDs-reuslts, with 2 groups each
  shape1 = getGEOMShape(0)
  assert(shape1)
  assert(shape1.GetName() == "Extrusion_1_1")
  assert(geompy.NumberOfSolids(shape1) == 1)
  assert(salome.ObjectToSObject(shape1).FindSubObject(1)[1])
  assert(salome.ObjectToSObject(shape1).FindSubObject(1)[1].GetName() == "Group_1")
  assert(salome.ObjectToSObject(shape1).FindSubObject(2)[1])
  assert(salome.ObjectToSObject(shape1).FindSubObject(2)[1].GetName() == "GroupSubstraction_1")

  shape2 = getGEOMShape(1)
  assert(shape2)
  assert(shape2.GetName() == "Extrusion_1_2")
  assert(geompy.NumberOfSolids(shape2) == 1)
  assert(salome.ObjectToSObject(shape2).FindSubObject(1)[1])
  assert(salome.ObjectToSObject(shape2).FindSubObject(1)[1].GetName() == "Group_1")
  assert(salome.ObjectToSObject(shape2).FindSubObject(2)[1])
  assert(salome.ObjectToSObject(shape2).FindSubObject(2)[1].GetName() == "Group_2")

  shape3 = getGEOMShape(2)
  assert(shape3)
  assert(shape3.GetName() == "Extrusion_1_3")
  assert(geompy.NumberOfSolids(shape3) == 1)
  assert(salome.ObjectToSObject(shape3).FindSubObject(1)[1])
  assert(salome.ObjectToSObject(shape3).FindSubObject(1)[1].GetName() == "Group_1")
  assert(salome.ObjectToSObject(shape3).FindSubObject(2)[1])
  assert(salome.ObjectToSObject(shape3).FindSubObject(2)[1].GetName() == "GroupSubstraction_1")

  shape4 = getGEOMShape(3)
  assert(not shape4)


if __name__ == '__main__':
  testExportToGEOM()
