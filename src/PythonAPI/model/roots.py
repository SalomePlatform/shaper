"""Abstract root classes of user-defined Python features producing a Body
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
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

