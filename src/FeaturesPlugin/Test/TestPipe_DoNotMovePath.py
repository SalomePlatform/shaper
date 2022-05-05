# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(-592.0852359208525, -262.5570776255708, -625.5707762557078, -186.4535768645358)
SketchLine_1.setAuxiliary(True)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(-625.5707762557078, -186.4535768645358, -639.2694063926941, -108.8280060882801)
SketchLine_2.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(-639.2694063926941, -108.8280060882801, -637.7473363774735, -9.893455098934579)
SketchLine_3.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(-637.7473363774735, -9.893455098934579, -614.9162861491628, 101.2176560121766)
SketchLine_4.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(-614.9162861491628, 101.2176560121766, -573.8203957382038, 200.1522070015221)
SketchLine_5.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(-573.8203957382038, 200.1522070015221, -532.724505327245, 200.1522070015221)
SketchLine_6.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(-532.724505327245, 200.1522070015221, 80.66971080669715, 200.1522070015221)
SketchLine_7.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
Sketch_1.setHorizontal(SketchLine_7.result())

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(80.66971080669715, 200.1522070015221, 68.49315068493158, 101.2176560121766)
SketchLine_8.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())

### Create SketchLine
SketchLine_9 = Sketch_1.addLine(68.49315068493158, 101.2176560121766, 54.79452054794545, -9.893455098934579)
SketchLine_9.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())

### Create SketchLine
SketchLine_10 = Sketch_1.addLine(54.79452054794545, -9.893455098934579, 63.92694063926946, -108.8280060882801)
SketchLine_10.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())

### Create SketchLine
SketchLine_11 = Sketch_1.addLine(63.92694063926946, -108.8280060882801, 66.97108066971103, -186.4535768645358)
SketchLine_11.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())

### Create SketchLine
SketchLine_12 = Sketch_1.addLine(66.97108066971103, -186.4535768645358, 92.84627092846273, -262.5570776255708)
SketchLine_12.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())

### Create SketchLine
SketchLine_13 = Sketch_1.addLine(92.84627092846273, -262.5570776255708, -546.4231354642313, -262.5570776255708)
SketchLine_13.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
Sketch_1.setHorizontal(SketchLine_13.result())

### Create SketchLine
SketchLine_14 = Sketch_1.addLine(-546.4231354642313, -262.5570776255708, -584.4748858447489, -186.4535768645358)
SketchLine_14.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())

### Create SketchLine
SketchLine_15 = Sketch_1.addLine(-584.4748858447489, -186.4535768645358, -593.6073059360729, -108.8280060882801)
SketchLine_15.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())

### Create SketchLine
SketchLine_16 = Sketch_1.addLine(-593.6073059360729, -108.8280060882801, -573.8203957382038, -9.893455098934577)
SketchLine_16.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())

### Create SketchLine
SketchLine_17 = Sketch_1.addLine(-573.8203957382038, -9.893455098934577, -558.599695585997, 101.2176560121766)
SketchLine_17.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())

### Create SketchLine
SketchLine_18 = Sketch_1.addLine(-558.599695585997, 101.2176560121766, -532.724505327245, 200.1522070015221)
SketchLine_18.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_18.endPoint())

### Create SketchLine
SketchLine_19 = Sketch_1.addLine(-592.0852359208525, -262.5570776255708, -546.4231354642313, -262.5570776255708)
SketchLine_19.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_19.startPoint())
Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_19.endPoint())

### Create SketchLine
SketchLine_20 = Sketch_1.addLine(-625.5707762557078, -186.4535768645358, -584.4748858447489, -186.4535768645358)
SketchLine_20.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_20.startPoint())
Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_20.endPoint())

### Create SketchLine
SketchLine_21 = Sketch_1.addLine(-639.2694063926941, -108.8280060882801, -593.6073059360729, -108.8280060882801)
SketchLine_21.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_21.startPoint())
Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_21.endPoint())

### Create SketchLine
SketchLine_22 = Sketch_1.addLine(-637.7473363774735, -9.893455098934579, -573.8203957382038, -9.893455098934577)
SketchLine_22.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_22.startPoint())
Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_22.endPoint())
Sketch_1.setHorizontal(SketchLine_22.result())

### Create SketchLine
SketchLine_23 = Sketch_1.addLine(-614.9162861491628, 101.2176560121766, -558.599695585997, 101.2176560121766)
SketchLine_23.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_23.startPoint())
Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_23.endPoint())

### Create SketchLine
SketchLine_24 = Sketch_1.addLine(-558.599695585997, 101.2176560121766, 68.49315068493158, 101.2176560121766)
SketchLine_24.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_24.startPoint())
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_24.endPoint())

### Create SketchLine
SketchLine_25 = Sketch_1.addLine(-573.8203957382038, -9.893455098934577, 54.79452054794545, -9.893455098934579)
SketchLine_25.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_25.startPoint())
Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_25.endPoint())
Sketch_1.setHorizontal(SketchLine_25.result())

### Create SketchLine
SketchLine_26 = Sketch_1.addLine(-593.6073059360729, -108.8280060882801, 63.92694063926946, -108.8280060882801)
SketchLine_26.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_26.startPoint())
Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_26.endPoint())
Sketch_1.setHorizontal(SketchLine_26.result())

### Create SketchLine
SketchLine_27 = Sketch_1.addLine(-584.4748858447489, -186.4535768645358, 66.97108066971103, -186.4535768645358)
SketchLine_27.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_27.startPoint())
Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_27.endPoint())
Sketch_1.setHorizontal(SketchLine_27.result())
Sketch_1.setHorizontal(SketchLine_6.result())
Sketch_1.setHorizontal(SketchLine_23.result())
Sketch_1.setHorizontal(SketchLine_24.result())
Sketch_1.setHorizontal(SketchLine_21.result())
Sketch_1.setHorizontal(SketchLine_20.result())
Sketch_1.setHorizontal(SketchLine_19.result())
model.do()

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("VERTEX", "Sketch_1/SketchLine_13_StartVertex"))

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("VERTEX", "Sketch_1/SketchLine_11_EndVertex"))

### Create Plane
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("VERTEX", "Sketch_1/SketchLine_11_StartVertex"))

### Create Plane
Plane_7 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("VERTEX", "Sketch_1/SketchLine_10_StartVertex"))

### Create Plane
Plane_8 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("VERTEX", "Sketch_1/SketchLine_24_EndVertex"))

### Create Plane
Plane_9 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("VERTEX", "Sketch_1/SketchLine_7_EndVertex"))

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "all-in-Plane_1"))

### Create SketchProjection
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_13_StartVertex"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchProjection
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_19_EndVertex"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()

### Create SketchCircle
SketchCircle_1 = Sketch_2.addCircle(92.84627092846273, 0, 639.269406392694)
Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
Sketch_2.setCoincident(SketchPoint_2.result(), SketchCircle_1.results()[1])

### Create SketchProjection
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()

### Create SketchCircle
SketchCircle_2 = Sketch_2.addCircle(96.29157651318411, -4.925446150925168, 688.3944333858536)
Sketch_2.setCoincident(SketchPoint_3.result(), SketchCircle_2.results()[1])
model.do()

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "all-in-Plane_2"))

### Create SketchProjection
SketchProjection_4 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_11_EndVertex"), False)
SketchPoint_4 = SketchProjection_4.createdFeature()

### Create SketchProjection
SketchProjection_5 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_20_EndVertex"), False)
SketchPoint_5 = SketchProjection_5.createdFeature()

### Create SketchCircle
SketchCircle_3 = Sketch_3.addCircle(66.97108066971103, 0, 651.4459665144599)
Sketch_3.setCoincident(SketchPoint_4.result(), SketchCircle_3.center())
Sketch_3.setCoincident(SketchPoint_5.result(), SketchCircle_3.results()[1])

### Create SketchProjection
SketchProjection_6 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), False)
SketchPoint_6 = SketchProjection_6.createdFeature()

### Create SketchCircle
SketchCircle_4 = Sketch_3.addCircle(66.97108066971103, 0, 692.5418569254188)
Sketch_3.setCoincident(SketchAPI_Point(SketchPoint_4).coordinates(), SketchCircle_4.center())
Sketch_3.setCoincident(SketchPoint_6.result(), SketchCircle_4.results()[1])
model.do()

### Create Sketch
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "all-in-Plane_3"))

### Create SketchProjection
SketchProjection_7 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_11_StartVertex"), False)
SketchPoint_7 = SketchProjection_7.createdFeature()

### Create SketchProjection
SketchProjection_8 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_15_EndVertex"), False)
SketchPoint_8 = SketchProjection_8.createdFeature()

### Create SketchCircle
SketchCircle_5 = Sketch_4.addCircle(63.92694063926946, 0, 657.5342465753423)
Sketch_4.setCoincident(SketchPoint_7.result(), SketchCircle_5.center())
Sketch_4.setCoincident(SketchPoint_8.result(), SketchCircle_5.results()[1])

### Create SketchProjection
SketchProjection_9 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_21_StartVertex"), False)
SketchPoint_9 = SketchProjection_9.createdFeature()

### Create SketchCircle
SketchCircle_6 = Sketch_4.addCircle(63.92694063926946, 0, 703.1963470319636)
Sketch_4.setCoincident(SketchAPI_Point(SketchPoint_7).coordinates(), SketchCircle_6.center())
Sketch_4.setCoincident(SketchPoint_9.result(), SketchCircle_6.results()[1])
model.do()

### Create Sketch
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "all-in-Plane_4"))

### Create SketchProjection
SketchProjection_10 = Sketch_5.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_10_StartVertex"), False)
SketchPoint_10 = SketchProjection_10.createdFeature()

### Create SketchProjection
SketchProjection_11 = Sketch_5.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_22_EndVertex"), False)
SketchPoint_11 = SketchProjection_11.createdFeature()

### Create SketchCircle
SketchCircle_7 = Sketch_5.addCircle(54.79452054794545, 0, 628.6149162861493)
Sketch_5.setCoincident(SketchPoint_10.result(), SketchCircle_7.center())
Sketch_5.setCoincident(SketchPoint_11.result(), SketchCircle_7.results()[1])

### Create SketchProjection
SketchProjection_12 = Sketch_5.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_4_StartVertex"), False)
SketchPoint_12 = SketchProjection_12.createdFeature()

### Create SketchCircle
SketchCircle_8 = Sketch_5.addCircle(54.79452054794545, 0, 692.541856925419)
Sketch_5.setCoincident(SketchAPI_Point(SketchPoint_10).coordinates(), SketchCircle_8.center())
Sketch_5.setCoincident(SketchPoint_12.result(), SketchCircle_8.results()[1])
model.do()

### Create Sketch
Sketch_6 = model.addSketch(Part_1_doc, model.selection("FACE", "all-in-Plane_5"))

### Create SketchProjection
SketchProjection_13 = Sketch_6.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_24_EndVertex"), False)
SketchPoint_13 = SketchProjection_13.createdFeature()

### Create SketchProjection
SketchProjection_14 = Sketch_6.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_18_StartVertex"), False)
SketchPoint_14 = SketchProjection_14.createdFeature()

### Create SketchCircle
SketchCircle_9 = Sketch_6.addCircle(68.49315068493158, 0, 627.0928462709286)
Sketch_6.setCoincident(SketchPoint_13.result(), SketchCircle_9.center())
Sketch_6.setCoincident(SketchPoint_14.result(), SketchCircle_9.results()[1])

### Create SketchProjection
SketchProjection_15 = Sketch_6.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_23_StartVertex"), False)
SketchPoint_15 = SketchProjection_15.createdFeature()

### Create SketchCircle
SketchCircle_10 = Sketch_6.addCircle(68.49315068493158, 0, 683.4094368340944)
Sketch_6.setCoincident(SketchAPI_Point(SketchPoint_13).coordinates(), SketchCircle_10.center())
Sketch_6.setCoincident(SketchPoint_15.result(), SketchCircle_10.results()[1])
model.do()

### Create Sketch
Sketch_7 = model.addSketch(Part_1_doc, model.selection("FACE", "all-in-Plane_6"))

### Create SketchProjection
SketchProjection_16 = Sketch_7.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_7_EndVertex"), False)
SketchPoint_16 = SketchProjection_16.createdFeature()

### Create SketchProjection
SketchProjection_17 = Sketch_7.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_18_EndVertex"), False)
SketchPoint_17 = SketchProjection_17.createdFeature()

### Create SketchCircle
SketchCircle_11 = Sketch_7.addCircle(80.66971080669715, 0, 613.3942161339421)
Sketch_7.setCoincident(SketchPoint_16.result(), SketchCircle_11.center())
Sketch_7.setCoincident(SketchPoint_17.result(), SketchCircle_11.results()[1])

### Create SketchProjection
SketchProjection_18 = Sketch_7.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_6_StartVertex"), False)
SketchPoint_18 = SketchProjection_18.createdFeature()

### Create SketchCircle
SketchCircle_12 = Sketch_7.addCircle(80.66971080669715, 0, 654.490106544901)
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_16).coordinates(), SketchCircle_12.center())
Sketch_7.setCoincident(SketchPoint_18.result(), SketchCircle_12.results()[1])
model.do()

### Create Wire
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_8"),
                  model.selection("EDGE", "Sketch_1/SketchLine_9"),
                  model.selection("EDGE", "Sketch_1/SketchLine_10"),
                  model.selection("EDGE", "Sketch_1/SketchLine_11"),
                  model.selection("EDGE", "Sketch_1/SketchLine_12")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)

### Create Pipe
Pipe_1_objects_1 = [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2f"),
                    model.selection("FACE", "Sketch_3/Face-SketchCircle_3_2f"),
                    model.selection("FACE", "Sketch_4/Face-SketchCircle_5_2f"),
                    model.selection("FACE", "Sketch_5/Face-SketchCircle_7_2f"),
                    model.selection("FACE", "Sketch_6/Face-SketchCircle_9_2f"),
                    model.selection("FACE", "Sketch_7/Face-SketchCircle_11_2f")]
Pipe_1 = model.addPipe(Part_1_doc, Pipe_1_objects_1, model.selection("WIRE", "Wire_1_1"), [])

### Create Wire
Wire_2_objects = [model.selection("EDGE", "Sketch_1/SketchLine_8"),
                  model.selection("EDGE", "Sketch_1/SketchLine_9"),
                  model.selection("EDGE", "Sketch_1/SketchLine_10"),
                  model.selection("EDGE", "Sketch_1/SketchLine_11"),
                  model.selection("EDGE", "Sketch_1/SketchLine_12")]
Wire_2 = model.addWire(Part_1_doc, Wire_2_objects)

### Create Pipe
Pipe_2_objects_1 = [model.selection("EDGE", "Sketch_2/SketchCircle_2_2"),
                    model.selection("EDGE", "Sketch_3/SketchCircle_4_2"),
                    model.selection("EDGE", "Sketch_4/SketchCircle_6_2"),
                    model.selection("EDGE", "Sketch_5/SketchCircle_8_2"),
                    model.selection("EDGE", "Sketch_6/SketchCircle_10_2"),
                    model.selection("EDGE", "Sketch_7/SketchCircle_12_2")]
Pipe_2 = model.addPipe(Part_1_doc, Pipe_2_objects_1, model.selection("WIRE", "Wire_2_1"), [])

model.end()

# Check results
from GeomAPI import GeomAPI_Shape

model.testNbResults(Pipe_1, 1)
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.FACE, [7])
model.testResultsAreas(Pipe_1, [4399065.64794281])

model.testNbResults(Pipe_2, 1)
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.FACE, [5])
model.testResultsAreas(Pipe_2, [2063250.44226183])
