# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

"""
    Test1924.py
    Test case for issue #1924 "Wrong naming of multiple-rotated sketch edges on python dump"
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(70, 120, 50, 25)
SketchLine_2 = Sketch_1.addLine(50, 25, 170, 50)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchMultiRotation_1_objects = [SketchLine_1.result(), SketchLine_2.result()]
# names of rotated objects: "SketchLine_3", "SketchLine_4"
SketchMultiRotation_1 = Sketch_1.addRotation(SketchMultiRotation_1_objects, SketchPoint_1.coordinates(), 90, 2)
# line named "SketchLine_5"
SketchLine_5 = Sketch_1.addLine(0, 100, 100, 100)
# change number of rotated objects.
# names of rotated objects: "SketchLine_3", "SketchLine_4", "SketchLine_6", "SketchLine_7"
SketchMultiRotation_1.numberOfObjects().setValue(3)

model.end()

assert(model.checkPythonDump())
