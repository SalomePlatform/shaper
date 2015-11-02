"""Part Feature Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import ModelAPI

from model.roots import Interface


def addPart(partset):
    """Add a Part feature to the Part and return Part.

    Pass all args to Part __init__ function.
    """
    feature = partset.addFeature("Part")
    return Part(feature)

def duplicatePart(part):
    """Create a copy of the Part."""
    feature = part.addFeature("Duplicate")
    feature.execute()
    return Part(feature)

def removePart(part):
    """Remove the Part."""
    feature = part.addFeature("Remove")
    feature.execute()


class Part(Interface):
    """Interface class for Part feature.

    Part(feature) -> feature interface without initialization
    """

    def __init__(self, feature):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Part")

        self._execute()
        pass

    def document(self):
        """Returns the Part document created by this feature."""
        result_part = ModelAPI.modelAPI_ResultPart(self._feature.firstResult())
        return result_part.partDoc()
