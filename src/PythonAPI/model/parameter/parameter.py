"""Parameter Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addParameter(part, *args):
    """Add a Parameter feature to the Part and return Parameter.

    Pass all args to Parameter __init__ function.
    """
    assert(args)
    feature = part.addFeature("Parameter")
    return Parameter(feature, *args)


class Parameter(Interface):
    """Interface class for Parameter feature.

    Parameter(feature) -> feature interface without initialization
    Parameter(feature, variable, expression) ->
        feature interface initialized from arguments:
        - variable -- variable name
        - expression -- Python expression
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Parameter")

        self._variable = self._feature.data().string("variable")
        self._expression = self._feature.data().string("expression")

        assert(self._variable)
        assert(self._expression)

        if not args:
            return

        assert(len(args) == 2)
        self.setName(args[0])
        self.setExpression(args[1])

        self.execute()
        pass

    def setName(self, name):
        """Modify variable name attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._name, name)
        pass

    def setExpression(self, expression):
        """Modify expression attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._expression, expression)
        pass
