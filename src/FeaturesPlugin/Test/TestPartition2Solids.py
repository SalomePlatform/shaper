## Copyright (C) 2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##


from salome.shaper import model
from GeomAPI import GeomAPI_Shape

model.begin()
#Create part
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
#Create Sketch_1 - closed polyline from 5 segments
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(-43.90694444444443, 28.05555555555554, 19.21805555555554, 72.66388888888889)
SketchLine_2 = Sketch_1.addLine(19.21805555555554, 72.66388888888889, 82.06249999999999, -36.19166666666666)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(82.06249999999999, -36.19166666666666, 19.4986111111111, -81.3611111111111)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(19.4986111111111, -81.3611111111111, -45.87083333333333, -45.44999999999999)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-45.87083333333333, -45.44999999999999, -43.90694444444443, 28.05555555555554)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
model.do()
#Create Extrusion_1 on Sketch_1
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2r-SketchLine_3r-SketchLine_4r-SketchLine_5r")], model.selection(), 100, 0)
#Create Sketch_2 from 5 segments
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_3"))
SketchLine_6 = Sketch_2.addLine(74.07000206783135, -18.10965884648514, 62.37497281453477, -31.70853007124862)
SketchLine_7 = Sketch_2.addLine(62.37497281453477, -31.70853007124862, 37.35304976096997, -31.43655264675337)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(37.35304976096997, -31.43655264675337, 25.65802050767337, -1.791013376768974)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_2.addLine(25.65802050767337, -1.791013376768974, 69.44638585141178, 6.640286782584385)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_2.addLine(69.44638585141178, 6.640286782584385, 74.07000206783135, -18.10965884648514)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_6.startPoint(), SketchLine_10.endPoint())
model.do()
#Create Extrusion_2 on Sketch_2
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_6r-SketchLine_7r-SketchLine_8r-SketchLine_9r-SketchLine_10r")], model.selection(), 10, 160)
#Create Partition on objects Extrusion_1 and Extrusion_2
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1")])
model.do()

model.checkResult(Partition_1,model,1,[4],[4],[33],[150],[300])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
