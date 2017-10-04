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

from ModelAPI import *

aSession = ModelAPI_Session.get()
aDoc = aSession.moduleDocument()

aSession.startOperation()
aFeature = aDoc.addFeature("Point")
aFeatureData = aFeature.data()
assert(aFeatureData is not None)
# aFeatureData.string("creation_method").setValue("by_xyz")
aFeatureData.real("x").setValue(0.)
aFeatureData.real("y").setValue(0.)
aFeatureData.real("z").setValue(0.)
aFeatureData.string("creation_method").setValue("by_xyz")
aFeatureName = aFeature.name()
aFeature.execute()
aSession.finishOperation()

# with index "0" is Origin and then - all default constructions
aFeature1 = aDoc.object("Construction", 7)
aFeature1Name = aFeature1.data().name()

assert (aFeatureName == aFeature1Name)

from salome.shaper import model
assert(model.checkPythonDump())
