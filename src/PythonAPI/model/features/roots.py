
from model.roots import Interface


class CompositeBoolean(Interface):

    def __init__(self, feature, *args):
        Interface.__init__(self, feature)

        self._sketch = self._feature.reference("sketch")
        self._sketch_selection = self._feature.selection("sketch_selection")
        self._boolean_objects = self._feature.selectionList("boolean_objects")

        assert(self._sketch)
        assert(self._sketch_selection)
        assert(self._boolean_objects)

        if not args:
            return

        assert(len(args) == 3)
        sketch, sketch_selection, boolean_objects = args

        self.setSketch(sketch)
        self.setSketchSelection(sketch_selection)
        self.setBooleanObjects(boolean_objects)
        pass

    def setSketch(self, sketch):
        self._fill_attribute(self._sketch, sketch)
        pass

    def setSketchSelection(self, sketch_selection):
        self._fill_attribute(self._sketch_selection, sketch_selection)
        pass

    def setBooleanObjects(self, boolean_objects):
        self._fill_attribute(self._boolean_objects, boolean_objects)
        pass


class CompositeSketch(Interface):

    def __init__(self, feature, *args):
        Interface.__init__(self, feature)

        self._sketch = self._feature.reference("sketch")
        self._sketch_selection = self._feature.selection("sketch_selection")

        assert(self._sketch)
        assert(self._sketch_selection)

        if not args:
            return

        assert(len(args) == 2)
        sketch, sketch_selection = args

        self.setSketch(sketch)
        self.setSketchSelection(sketch_selection)
        pass

    def setSketch(self, sketch):
        self._fill_attribute(self._sketch, sketch)
        pass

    def setSketchSelection(self, sketch_selection):
        self._fill_attribute(self._sketch_selection, sketch_selection)
        pass
