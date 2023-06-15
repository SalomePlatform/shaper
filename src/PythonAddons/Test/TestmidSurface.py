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

theFile = getFilePath("midSurface.stp")
print ("theFile = {}".format(theFile))

print ("Create part for mid surface")
aSession.startOperation("Create part for mid surface")
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()

print ("Import file")
aSession.startOperation("Import file")
aFeatureKind = "midSurface"
anImportFeature = aPart.addFeature(aFeatureKind)
aFieldName = "file_path"
aFile = anImportFeature.string(aFieldName)
aFile.setValue(theFile)
aSession.finishOperation()

print ("\nNombre de dossiers : {}".format(aPart.size("Folders")))

assert(aPart.size("Folders") == 1), "Wrong number of folders: {}".format(aPart.size("Folders"))
