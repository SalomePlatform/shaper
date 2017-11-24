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

#=========================================================================
# Test checks stability of the foldering mechanism due to incorrect input parameters
#=========================================================================
from ModelAPI import *

__updated__ = "2017-11-23"

aSession = ModelAPI_Session.get()


def newPoint(theDocument, theX, theY, theZ):
    aSession.startOperation()
    aPoint = theDocument.addFeature("Point")
    aPointData = aPoint.data()
    assert(aPointData is not None)
    aPointData.real("x").setValue(theX)
    aPointData.real("y").setValue(theY)
    aPointData.real("z").setValue(theZ)
    aPointData.string("creation_method").setValue("by_xyz")
    aSession.finishOperation()
    return aPoint


#=========================================================================
# Initialization of the model
#=========================================================================
aSession.startOperation()
aPart = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()

# add point and a folder before it
aPartDoc = aSession.activeDocument()
aPoint1 = newPoint(aPartDoc, 0., 0., 0.)

aSession.startOperation()
aFolder1 = aPartDoc.addFolder(aPoint1)
aSession.finishOperation()

NB_FEATURES_FULL = 2
NB_FEATURES_OUT  = 2

assert(aPartDoc.size("Folders") == 1), "Wrong number of folders: {}".format(aPartDoc.size("Folders"))
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}".format(aPartDoc.size("Features"))

#=========================================================================
# Test 1. Check number of features out of folder
#         and absense of the crash while getting size of incorrect groupd
#=========================================================================
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)
assert(aPartDoc.size("Construction", True) == 1), "Wrong size: {}".format(aPartDoc.size("Construction", True))


from salome.shaper import model
assert(model.checkPythonDump())
