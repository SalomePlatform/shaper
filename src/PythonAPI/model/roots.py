"""Abstract root classes of user-defined Python features producing a Body
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import ModelAPI

from model import tools


class Feature(ModelAPI.ModelAPI_Feature):
    """Base class of user-defined Python features."""

    def __init__(self):
        ModelAPI.ModelAPI_Feature.__init__(self)

    def addRealInput (self, inputid):
        self.data().addAttribute(inputid,
                                 ModelAPI.ModelAPI_AttributeDouble_typeId())

    def getRealInput (self, inputid):
        return self.data().real(inputid).value()

    def addResult (self, result):
        shape = result.shape()
        body = self.document().createBody(self.data())
        body.store(shape)
        self.setResult(body)


class Interface():
    """Base class of high level Python interfaces to features."""

    def __init__(self, feature):
        self._feature = feature
        self._attribute_white_list = [
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

    def _fill_attribute(self, attribute, value, *args):
        """Fill ModelAPI_Attribute* with value."""
        tools.fill_attribute(attribute, value, *args)

    def setRealInput(self, inputid, value):
        self._feature.data().real(inputid).setValue(value)

    def areInputValid(self):
        validators = ModelAPI.ModelAPI_Session.get().validators()
        return validators.validate(self._feature)

    def execute(self):
        self._feature.execute()
