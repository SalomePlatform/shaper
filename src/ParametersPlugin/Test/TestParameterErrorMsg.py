# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
# check error on empty name
try:
    Param1 = model.addParameter(partSet, "", "100")
except SyntaxError as anError:
    assert(str(anError).find("Attribute \"variable\" value is empty.") != -1)
# check error on empty value
try:
    Param2 = model.addParameter(partSet, "L", "")
except SyntaxError as anError:
    assert(str(anError).find("Expression is empty.") != -1)
# check error if name is not variable
try:
    Param3 = model.addParameter(partSet, "100", "100")
except SyntaxError as anError:
    assert(str(anError).find("Incorrect variable name.") != -1)
# check error on wrong value expression
try:
    Param4 = model.addParameter(partSet, "N", "+-.so&@")
except SyntaxError as anError:
    assert(str(anError).find("invalid syntax") != -1)
model.end()
