## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

# Created a Part Remove/Duplicate API functionality since from low level and python dump it is not
# tested: feature history infor is not stored.

from salome.shaper import model

# prepare axis created by PartSet and Part points
model.begin()
PartSet = model.moduleDocument()
Part_1 = model.addPart(PartSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 100, 50, 20)
Axis_4 = model.addAxis(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("VERTEX", "Point_1"))
model.do()

# Duplicate this part twice, duplicate the first Copy also
Copy_1 = model.duplicatePart(Part_1)
Copy_2 = model.duplicatePart(Part_1)
Copy_3 = model.duplicatePart(Copy_1)
model.do()

# check the intermediate result
assert PartSet.size("Parts") == 4
i = 0
for n in ["Part_1", "Part_3", "Part_2", "Part_4"]:
  assert PartSet.object("Parts", i).data().name() == n
  i = i + 1

# Remove the middle Part
model.removePart(Copy_2)
model.do()

# check the intermediate result
assert PartSet.size("Parts") == 3
i = 0
for n in ["Part_1", "Part_2", "Part_4"]:
  assert PartSet.object("Parts", i).data().name() == n
  i = i + 1

# Remove a first copy
model.removePart(Copy_1)
model.end()

# Undo the last remove
model.undo()

# Check the result
assert PartSet.size("Parts") == 3
i = 0
for n in ["Part_1", "Part_2", "Part_4"]:
  assert PartSet.object("Parts", i).data().name() == n
  i = i + 1
