# Copyright (C) 2016-2020  CEA/DEN, EDF R&D
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
"""Package for services for the Parametric Geometry API of the Modeler.
"""

from ModelHighAPI import moduleDocument, activeDocument
from ModelHighAPI import defaultPlane, standardPlane
from ModelHighAPI import begin, end
from ModelHighAPI import apply as do
from ModelHighAPI import updateFeatures
from ModelHighAPI import undo, redo
from ModelHighAPI import reset
from ModelHighAPI import addFolder, removeFolder
from ModelHighAPI import ModelHighAPI_Selection as selection
from ModelAPI import findPartFeature

# a method used for the python dump of the SHAPER STUDY
def publishToShaperStudy():
  begin()
  activeDocument().addFeature("PublishToStudy")
  end()

# returns unique identifier of the feature : id of part it belongs to + ":" + id of feature
# the second argument may be the number of result if feature has more than one result (1 corresponds to the second result, etc)
def featureStringId(theFeature, *theArgs):
  aRoot = moduleDocument()
  aCurrent = theFeature.feature().document()
  if aRoot and aCurrent:
    aRes = str(findPartFeature(aRoot, aCurrent).data().featureId()) + ":" + str(theFeature.feature().data().featureId())
    if len(theArgs) == 1:
      aRes += ":" + str(theArgs[0])
    return aRes
  return ""


import os

def getTmpFileName(thePrefix, theSuffix):
  import tempfile
  tempdir = tempfile.gettempdir()
  tmp_file = tempfile.NamedTemporaryFile(suffix=theSuffix, prefix=thePrefix, dir=tempdir, delete=False)
  tmp_filename = tmp_file.name
  if os.name == "nt":
    tmp_filename.replace("\\", "/")
  tmp_file.close()
  return tmp_filename

def removeTmpFile(theFilename):
  try: os.remove(theFilename)
  except OSError: pass

# Verify the Python dump with generating the auxiliary filenames
from ModelHighAPI import CHECK_NAMING_AND_GEOMETRICAL
from ModelHighAPI import checkPyDump
def checkPythonDump(theDumpMode = CHECK_NAMING_AND_GEOMETRICAL):
  aPrefix = getTmpFileName("check_dump_", '')
  aNaming = aPrefix + ".py"
  aGeo  = aPrefix + "_geo.py"
  aWeak = aPrefix + "_weak.py"
  isOk = checkPyDump(aNaming, aGeo, aWeak, theDumpMode)
  removeTmpFile(aNaming)
  removeTmpFile(aGeo)
  removeTmpFile(aWeak)
  return isOk
