"""Abstract root classes of user-defined Python features producing a Body
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import ModelAPI

from model import tools


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

    def addResult(self, result):
        """F.addResult(ModelAPI_Result) -- add ModelAPI_Result shape as a result"""
        shape = result.shape()
        body = self.document().createBody(self.data())
        body.store(shape)
        self.setResult(body)


class Interface():
    """Base class of high level Python interfaces to features."""

    def __init__(self, feature):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        self._feature = feature
        self._attribute_white_list = [
            "execute",
            "getKind", "results", "firstResult", "lastResult",
            ]

    def __getattr__(self, name):
        """Process missing attributes.

        Add get*() methods for access feature attributes.
        Redirect some missing attributes to the feature.
        """
        if name.startswith("get"):
            possible_names = [
                "_" + name[3:],
                "_" + tools.convert_to_underscore(name[3:]),
                ]
            for possible_name in possible_names:
                if hasattr(self, possible_name):
                    def getter():
                        return getattr(self, possible_name)
                    return getter

        if name in self._attribute_white_list:
            return getattr(self._feature, name)

        raise AttributeError()

    def feature(self):
        """Return ModelAPI_Feature."""
        return self._feature

    def _fill_attribute(self, attribute, value):
        """Fill ModelAPI_Attribute* with value."""
        tools.fill_attribute(attribute, value)

    def setRealInput(self, inputid, value):
        """I.setRealInput(str, float) -- set real value to the attribute"""
        self._feature.data().real(inputid).setValue(value)

    def areInputValid(self):
        """I.areInputValid() -> True or False validation result"""
        validators = ModelAPI.ModelAPI_Session.get().validators()
        return validators.validate(self._feature)

    def _execute(self):
        """I._execute() -- validate and execute the feature.

        Raises RuntimeError if validation fails.
        """
        if self.areInputValid():
            self._feature.execute()
        else:
            raise RuntimeError("Can not execute %s: %s" %
                               (self._feature.getKind(), self._feature.error())
                               )
