"""Export to GEOM Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def exportToGEOM(part):
    """Export the Part to GEOM module."""
    feature = part.addFeature("ExportToGEOM")
    feature.execute()