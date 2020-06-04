# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

# Test if the Model_AttributeSelection::newestContext method work in case
# the initial shell is concealed by multi-rotation (all-feature selection)
# and the same sketch is used by a next shell.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchCircle_1 = Sketch_1.addCircle(47.53111880904039, 63.23707920074999, 11.27757708463696)
model.do()
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")])
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Shell_1")], model.selection("EDGE", "PartSet/OZ"), 3, keepSubResults = True)
Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")])
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Shell_2.feature()))

assert(model.checkPythonDump())