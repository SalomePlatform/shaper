"""Common tools for other modules.
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import re
import ModelAPI

def convert_to_underscore(name):
    """Convert CamelCase to underscore_case."""
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def get_value(attribute):
    """Return simple type value from ModelAPI attribute if possible.
    
    ModelAPI_AttributeDouble -> double
    ModelAPI_AttributeString -> str
    """
    if (isinstance(attribute, ModelAPI.ModelAPI_AttributeDouble) or
            isinstance(attribute, ModelAPI.ModelAPI_AttributeString)):
        return attribute.value()

    print type(attribute)
    return attribute
