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

from salome.shaper import model
from ModelAPI import *

import os

aSession = ModelAPI_Session.get()

def getFilePath(fileName):
    path = os.path.join(os.getenv("SHAPER_ROOT_DIR"), "bin", "salome", "macros", "midSurface")
    return os.path.join(path, fileName)

theFile = getFilePath("midSurface_2.stp")

aSession.startOperation("Create part for mid surface")
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()

aSession.startOperation("Import file")
aFeatureKind = "midSurface"
anImportFeature = aPart.addFeature(aFeatureKind)
aFieldName = "file_path"
aFile = anImportFeature.string(aFieldName)
aFile.setValue(theFile)
aSession.finishOperation()

#for cle in ("Construction", "Bodies", "Groups", "Folders"):
  #print (cle,":",aPart.size(cle))

assert(aPart.size("Construction") == 32), "Right number of construction: {}".format(aPart.size("Construction"))
assert(aPart.size("Bodies") == 24), "Right number of construction: {}".format(aPart.size("Bodies"))
assert(aPart.size("Groups") == 10), "Right number of construction: {}".format(aPart.size("Groups"))
assert(aPart.size("Folders") == 13), "Wrong number of folders: {}".format(aPart.size("Folders"))
