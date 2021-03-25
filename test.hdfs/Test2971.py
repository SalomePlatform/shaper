# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

if __name__ == "__main__":
  parts = locals()
  for key in list(parts.keys()):
    if key.startswith("Part_"):
      part = parts[key]
      model.testNbResults(part, 1)
      model.testNbSubResults(part, [0])
      model.testNbSubShapes(part, GeomAPI_Shape.SOLID, [12])
      model.testNbSubShapes(part, GeomAPI_Shape.FACE, [103])
      model.testNbSubShapes(part, GeomAPI_Shape.EDGE, [475])
      model.testNbSubShapes(part, GeomAPI_Shape.VERTEX, [950])
      model.testResultsVolumes(part, [6487764903.02328777])

  assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
