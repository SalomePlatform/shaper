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

"""Test de la création de faces gauches"""

import salome

salome.salome_init()

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Point
l_noms = list()
l_coo = list()
l_coo.append((0, 0, 0))
l_coo.append((1, 8, 0))
l_coo.append((3, 10, 4))
l_coo.append((5, 5, 5))
l_coo.append((2, 3, 1))
for iaux, (coo_x,coo_y,coo_z) in enumerate(l_coo):
  point = model.addPoint(Part_1_doc, coo_x,coo_y,coo_z)
  nom = "P_{}".format(iaux)
  point.setName(nom)
  point.result().setName(nom)
  l_noms.append(nom)

### Create Interpolation
Interpolation_1_objects = list()
for nom in l_noms:
  Interpolation_1_objects.append(model.selection("VERTEX", "all-in-{}".format(nom)))
Interpolation_1 = model.addInterpolation(Part_1_doc, Interpolation_1_objects, True, False)

### Create Wire
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Interpolation_1_1")], False)

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Wire_1_1")])

model.end()

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
