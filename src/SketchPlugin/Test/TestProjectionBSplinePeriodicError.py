# Copyright (C) 2020  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()

### Create Sketch
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))

### Create SketchBSpline
SketchBSpline_1_poles = [(39.56361839352194, -91.51890167437574),
                         (70.21199762078012, -72.31423451653721),
                         (42.28382694294977, -41.98350738641304),
                         (67.28504300461098, -31.5470157767404),
                         (50.62385917039768, -3.473688794131494),
                         (86.7958072325187, 15.01252243343181)
                        ]
SketchBSpline_1 = Sketch_1.addSpline(poles = SketchBSpline_1_poles)

### Create SketchBSplinePeriodic
SketchBSplinePeriodic_1_poles = [(42.08285838974091, 63.92994506772382),
                                 (45.90231280862586, 109.3628979208621),
                                 (103.3091534085915, 74.17970450461429),
                                 (80.27079545144119, 67.78674592116529),
                                 (119.1440714423075, 30.71538726951938),
                                 (57.61745686692009, 20.1653082760922)
                                ]
SketchBSplinePeriodic_1 = Sketch_1.addSpline(poles = SketchBSplinePeriodic_1_poles, periodic = True)
model.do()

### Create Sketch
Sketch_2 = model.addSketch(partSet, model.defaultPlane("YOZ"))

### Create SketchProjection
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchBSpline_1"), True)
SketchBSpline_2 = SketchProjection_1.createdFeature()

### Create SketchProjection
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchBSplinePeriodic_1"), True)
model.do()
model.end()

assert(SketchProjection_1.feature().error() == "")
assert(SketchProjection_2.feature().error() != "")
