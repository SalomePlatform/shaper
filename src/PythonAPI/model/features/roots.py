
from model.roots import Interface


class CompositeBoolean(Interface):
    """Interface class for CompositeBoolean features.

    CompositeBoolean(feature) -> feature interface without initialization
    CompositeBoolean(feature, sketch, sketch_selection, boolean_objects) ->
        feature interface initialized from arguments:
        - sketch
        - sketch_selection
        - boolean_objects
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
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
        """Modify sketch attribute"""
        self._fillAttribute(self._sketch, sketch)
        pass

    def setSketchSelection(self, sketch_selection):
        """Modify sketch_selection attribute"""
        self._fillAttribute(self._sketch_selection, sketch_selection)
        pass

    def setBooleanObjects(self, boolean_objects):
        """Modify boolean_objects attribute"""
        self._fillAttribute(self._boolean_objects, boolean_objects)
        pass


class CompositeSketch(Interface):
    """Interface class for CompositeSketch features.

    CompositeSketch(feature) -> feature interface without initialization
    CompositeSketch(feature, sketch, sketch_selection) ->
        feature interface initialized from arguments:
        - sketch
        - sketch_selection
    """


    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
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
        """Modify sketch attribute"""
        self._fillAttribute(self._sketch, sketch)
        pass

    def setSketchSelection(self, sketch_selection):
        """Modify sketch_selection attribute"""
        self._fillAttribute(self._sketch_selection, sketch_selection)
        pass
