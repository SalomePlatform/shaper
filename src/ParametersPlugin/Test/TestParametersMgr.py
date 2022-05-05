# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

"""
    TestParametersMgr.py

    class ParametersPlugin_ParametersMgr
"""

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *

__updated__ = "2018-12-05"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

aSession.startOperation()
aKind = "ParametersMgr"
aParamMgr = aDocument.addFeature(aKind)
assert(aParamMgr is not None)
assert(aParamMgr.getKind() == aKind)
aSession.finishOperation()
