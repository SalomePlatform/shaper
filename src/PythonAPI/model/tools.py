"""Common tools for other modules.
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import re
import collections

import ModelAPI
import GeomAPI
import GeomDataAPI

# from .sketcher.sketch import Sketch


def convert_to_underscore(name):
    """Convert CamelCase to underscore_case."""
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


class Selection:
    """Class for selection.

    Selection() -> empty selection
    Selection(name, type) -> selection initialized with arguments:
        - name -- topological name
        - type -- type of the object
    Selection(context, shape) -> selection initialized with arguments:
        - context -- ModelAPI_Result object
        - shape -- GeomAPI_Shape shape
    """

    def __init__(self, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""

        if not args:
            self.args = (None, None)
            return

        if len(args) == 1 and isinstance(args[0], basestring):
            self.args = args
            return

        assert(len(args) > 1 and len(args) < 4)
        assert(isinstance(args[0], basestring) or
               isinstance(args[0], ModelAPI.ModelAPI_Result))
        if isinstance(args[0], basestring):
            assert(isinstance(args[1], basestring))
        elif isinstance(args[0], ModelAPI.ModelAPI_Result) or args[0] is None:
            assert(isinstance(args[1], GeomAPI.GeomAPI_Shape))
        self.args = args


def fill_attribute(attribute, value):
    """Set value to attribure.

    This function processes complex cases.
    ModelAPI_AttributeSelectionList can accept string, result + shape, list of
    strings and [result + shape]...
    ModelAPI_AttributeDouble can accept float and string
    """
    if (isinstance(attribute, ModelAPI.ModelAPI_AttributeBoolean) or
        isinstance(attribute, ModelAPI.ModelAPI_AttributeDocRef) or
        isinstance(attribute, ModelAPI.ModelAPI_AttributeInteger) or
        isinstance(attribute, ModelAPI.ModelAPI_AttributeReference)
        ):
        attribute.setValue(value)

    elif isinstance(attribute, ModelAPI.ModelAPI_AttributeString):
        attribute.setValue(str(value))

    elif isinstance(attribute, ModelAPI.ModelAPI_AttributeDouble):
        if isinstance(value, basestring):
            attribute.setText(value)
        else:
            attribute.setValue(value)

    elif isinstance(attribute, ModelAPI.ModelAPI_AttributeIntArray):
        attrubute.setSize(len(value))
        for i in range(len(value)):
            attrubute.setValue(i, value[i])

    elif isinstance(attribute, ModelAPI.ModelAPI_AttributeRefAttr):
        assert(isinstance(value, ModelAPI.ModelAPI_Attribute) or
               isinstance(value, ModelAPI.ModelAPI_Object))
        if isinstance(value, ModelAPI.ModelAPI_Attribute):
            attribute.setAttr(value)
        elif isinstance(value, ModelAPI.ModelAPI_Object):
            attribute.setObject(value)

    elif isinstance(attribute, ModelAPI.ModelAPI_AttributeRefList):
        attribute.clear()
        if not value:
            return

        assert(isinstance(value, collections.Iterable))
        for item in value:
            assert(isinstance(item, ModelAPI.ModelAPI_Object))
            attribute.append(item)

    elif isinstance(attribute, ModelAPI.ModelAPI_AttributeSelection):
        if value is None:
            attribute.setValue(None, None)
            return

        assert(isinstance(value, Selection))
        attribute.setValue(*value.args)

    elif isinstance(attribute, ModelAPI.ModelAPI_AttributeSelectionList):
        attribute.clear()
        if not value:
            return

        assert(isinstance(value, collections.Iterable))
        for item in value:
            assert(isinstance(item, Selection))
            attribute.append(*item.args)

    elif (isinstance(attribute, GeomDataAPI.GeomDataAPI_Dir) or
          isinstance(attribute, GeomDataAPI.GeomDataAPI_Point) or
          isinstance(attribute, GeomDataAPI.GeomDataAPI_Point2D)
          ):
        assert(isinstance(value, collections.Iterable))
        attribute.setValue(*value)

    else:
        raise AssertionError("Wrong attribute type: %s" % type(attribute))
