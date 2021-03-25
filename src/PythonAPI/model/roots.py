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

"""Abstract root classes of user-defined Python features producing a Body
Author: Daniel Brunier-Coulin
"""

import ModelAPI

class Feature(ModelAPI.ModelAPI_Feature):
    """Base class of user-defined Python features."""

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        ModelAPI.ModelAPI_Feature.__init__(self)

    def addRealInput(self, inputid):
        """F.addRealInput(str) -- add real attribute"""
        self.data().addAttribute(inputid,
                                 ModelAPI.ModelAPI_AttributeDouble_typeId())

    def getRealInput(self, inputid):
        """F.getRealInput(str) -- get real value of the attribute"""
        return self.data().real(inputid).value()

    def getTextInput(self, inputid):
        """F.getTextInput(str) -- get text value of the attribute"""
        return self.data().real(inputid).text()

    def addResult(self, result):
        """F.addResult(ModelAPI_Result) -- add ModelAPI_Result shape as a result"""
        shape = result.shape()
        body = self.document().createBody(self.data())
        body.store(shape)
        self.setResult(body)

