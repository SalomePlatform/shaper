"""Parameter Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addParameter(part, *args):
    """Add a Parameter feature to the Part and return Parameter.

    .. function:: addParameter(part, variable, expression)

    Args:
        part (ModelAPI_Document): part document
        variable (string): variable name
        expression (string): Python expression

    Returns:
        Parameter: parameter object

    Pass all args to Parameter __init__ function.
    """
    assert(args)
    feature = part.addFeature("Parameter")
    return Parameter(feature, *args)


class Parameter(Interface):
    """Interface class for Parameter feature.

    .. function:: Point(feature)

        Create interface for the feature without initialization.

    .. function:: Point(feature, variable, expression)

        Create interface for the feature and initialize the feature with arguments.
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Parameter")

        self._variable = self._feature.string("variable")
        self._expression = self._feature.string("expression")

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
        self._fillAttribute(self._variable, name)
        pass

    def setExpression(self, expression):
        """Modify expression attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._expression, expression)
        pass
