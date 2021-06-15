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
"""Test de la création du réseau de tuyaux - Description par ligne entière"""

__revision__ = "V01.04"

import os

from ModelAPI import *

aSession = ModelAPI_Session.get()

def getFilePath(fileName):
    """Le fichier décrivant le réseau"""
    path = os.path.join(os.getenv("SHAPER_ROOT_DIR"), "bin", "salome", "macros", "pipeNetwork")
    return os.path.join(path, fileName)

theFile = getFilePath("pipeNetwork_ligne.txt")

aSession.startOperation("Create part for pipe network")
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()

aSession.startOperation("Import file")
aFeatureKind = "pipeNetwork"
anImportFeature = aPart.addFeature(aFeatureKind)
aFieldName = "file_path"
aFile = anImportFeature.string(aFieldName)
aFile.setValue(theFile)
aSession.finishOperation()

assert(aPart.size("Construction") == 46), "Right number of construction: {}".format(aPart.size("Construction"))

assert(aPart.size("Folders") == 1), "Right number of folders: {}".format(aPart.size("Folders"))
