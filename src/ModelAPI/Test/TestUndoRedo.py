# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

from ModelAPI import *
from GeomDataAPI import *

aSession = ModelAPI_Session.get()
aDoc = aSession.moduleDocument()
assert(not aSession.canUndo())
assert(not aSession.canRedo())

aSession.startOperation()
aFeature = aDoc.addFeature("Point")
geomDataAPI_Point(aFeature.attribute("point3d")).setValue(1., -1., 0.)
aFeature.string("creation_method").setValue("by_xyz")
aFeatureName = aFeature.name()
# "2" is because Origin is the first point
assert(aFeatureName == "Point_2")

aFeature.execute()
aSession.finishOperation()

assert(aDoc.size("Construction") == 8)
assert(aSession.canUndo())
assert(not aSession.canRedo())

aSession.undo()
assert(aDoc.size("Construction") == 7)
assert(not aSession.canUndo())
assert(aSession.canRedo())

aSession.redo()
assert(aDoc.size("Construction") == 8)
assert(aSession.canUndo())
assert(not aSession.canRedo())

from salome.shaper import model
assert(model.checkPythonDump())
