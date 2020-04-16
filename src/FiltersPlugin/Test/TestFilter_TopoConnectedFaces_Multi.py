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

from SketchAPI import *
from GeomAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-21.08194442253927, 8.656344751543489, 0, 30)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(0, 30, 25.98076211353316, 15)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(25.98076211353316, 15, 18.03758667002239, -13.92932705531892)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(18.03758667002239, -13.92932705531892, -11.39496253934835, -19.73665406849542)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-11.39496253934835, -19.73665406849542, -21.08194442253927, 8.656344751543489)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_5.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_1.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_6.result(), SketchLine_3.result())
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_5.result())
SketchLine_7 = Sketch_1.addLine(-11.39496253934835, -19.73665406849542, 0, 0)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_7.endPoint())
SketchLine_8 = Sketch_1.addLine(-21.08194442253927, 8.656344751543489, 0, 0)
SketchLine_8.setAuxiliary(True)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_8.endPoint())
SketchLine_9 = Sketch_1.addLine(0, 30, 0, 0)
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_1.addLine(25.98076211353316, 15, 0, 0)
SketchLine_10.setAuxiliary(True)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_10.endPoint())
SketchLine_11 = Sketch_1.addLine(18.03758667002239, -13.92932705531892, 0, 0)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_11.endPoint())
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchLine_7.result(), SketchLine_8.result())
SketchConstraintEqual_6 = Sketch_1.setEqual(SketchLine_11.result(), SketchLine_7.result())
SketchConstraintEqual_7 = Sketch_1.setEqual(SketchLine_10.result(), SketchLine_9.result())
SketchConstraintEqual_8 = Sketch_1.setEqual(SketchLine_10.result(), SketchLine_3.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 30)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection(), 10, 0)
model.do()
# Two filters TopoConnectedFaces and not horizontal to search lateral face connected to two selected faces
Filters = model.filters(Part_1_doc, [
  model.addFilter(name = "TopoConnectedFaces", args = [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5"), False]),
  model.addFilter(name = "HorizontalFaces", exclude = True),
  model.addFilter(name = "TopoConnectedFaces", args = [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"), False])])
model.end()

Result = Extrusion_1.feature().firstResult()
exp = GeomAPI_ShapeExplorer(Result.shape(), GeomAPI_Shape.FACE)
Reference = {}
# 1 face is in result - between two selected faces for filters
Reference[model.selection(Result, exp.current())] = False; exp.next()
Reference[model.selection(Result, exp.current())] = False;  exp.next()
Reference[model.selection(Result, exp.current())] = True; exp.next()
Reference[model.selection(Result, exp.current())] = False;  exp.next()
Reference[model.selection(Result, exp.current())] = False;  exp.next()
Reference[model.selection(Result, exp.current())] = False;  exp.next()
Reference[model.selection(Result, exp.current())] = False;  exp.next()

model.checkFilter(Part_1_doc, model, Filters, Reference)
