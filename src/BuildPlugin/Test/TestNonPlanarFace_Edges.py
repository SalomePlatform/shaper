# Copyright (C) 2017-2023  CEA, EDF
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

import sys
import salome

salome.salome_init()
import salome_notebook
notebook = salome_notebook.NoteBook()
sys.path.insert(0, r'/home/eksu/S2')

###
### SHAPER component
###

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Points
Point_2 = model.addPoint(Part_1_doc, 0, 0, 0)
Point_3 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_4 = model.addPoint(Part_1_doc, 0, 10, 0)
Point_5 = model.addPoint(Part_1_doc, 10, 10, 3)

### Create Edges
Edge_1 = model.addEdge(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))
Edge_2 = model.addEdge(Part_1_doc, model.selection("VERTEX", "Edge_1_1/Modified_Vertex&Point_2/Point_2"), model.selection("VERTEX", "Point_4"))
Edge_3 = model.addEdge(Part_1_doc, model.selection("VERTEX", "Edge_2_1/Modified_Vertex&Point_4/Point_4"), model.selection("VERTEX", "Point_3"))
Edge_4 = model.addEdge(Part_1_doc, model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_1"))

### Create Wire
Wire_1_objects = [model.selection("EDGE", "Edge_1_1"),
                  model.selection("EDGE", "Edge_2_1"),
                  model.selection("EDGE", "Edge_4_1"),
                  model.selection("EDGE", "Edge_3_1")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)

### Create Face
Face_1_objects = [model.selection("EDGE", "Wire_1_1/Modified_Edge&Edge_1_1/Edge_1_1"),
                  model.selection("EDGE", "Wire_1_1/Modified_Edge&Edge_2_1/Edge_2_1"),
                  model.selection("EDGE", "Wire_1_1/Modified_Edge&Edge_4_1/Edge_4_1"),
                  model.selection("EDGE", "Wire_1_1/Modified_Edge&Edge_3_1/Edge_3_1")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)

model.end()

###
### SHAPERSTUDY component
###

model.publishToShaperStudy()
import SHAPERSTUDY
Face_1_1, = SHAPERSTUDY.shape(model.featureStringId(Face_1))

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
