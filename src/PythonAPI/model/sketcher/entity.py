
from model.roots import Interface

class Entity(Interface):
    """Interface for editing of a sketch entity feature."""

    def __init__(self, feature):
        Interface.__init__(self, feature)

        # Initialize attributes
        self._auxiliary = self._feature.boolean("Auxiliary")

    def setAuxiliary(self, a):
        """Set the Auxiliarity."""
        self._auxiliary.setValue(a)
