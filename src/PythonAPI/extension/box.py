"""Box macro-feature Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import model
from macros.box.feature import BoxFeature as MY


def addBox(self, container, *args):
    feature = container.addFeature(MY.ID())
    return Box(feature, *args)


class Box(model.Interface):
    """Executes the macro-feature Box."""

    def __init__(self, feature, dx, dy, dz):
        model.Interface.__init__(self, feature)

        self.setRealInput(MY.WIDTH_ID(), dx)
        self.setRealInput(MY.LENGTH_ID(), dy)
        self.setRealInput(MY.HEIGHT_ID(), dz)

        if self.areInputValid():
            self.execute()
        else:
            raise Exception("cannot make the Box")
