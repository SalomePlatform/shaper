

from model.roots import Interface


class CompositeBoolean(Interface):

    def __init__(self, feature, *args):
        Interface.__init__(self, feature)

        self._sketch = self._feature.reference("sketch")
        self._sketch_selection = self._feature.selection("sketch_selection")
        self._boolean_objects = self._feature.selectionList("boolean_objects")

        if not args:
            return

class CompositeSketch(Interface):

    def __init__(self, feature, *args):
        Interface.__init__(self, feature)

        self._sketch = self._feature.reference("sketch")
        self._sketch_selection = self._feature.selection("sketch_selection")

        if not args:
            return
