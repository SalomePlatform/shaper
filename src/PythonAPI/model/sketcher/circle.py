"""Sketch circle feature interface."""

from GeomDataAPI import geomDataAPI_Point2D
from model.roots import Interface

class Circle(Interface):
    """Interface for circle feature data manipulation."""
    def __init__(self, feature, *args):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "SketchCircle")
        
        self._center = geomDataAPI_Point2D(
            self._feature.data().attribute("CircleCenter")
            )
        self._radius = self._feature.data().real("CircleRadius")
        
        if not args:
            return
        
        if len(args) != 3:
            raise TypeError(
                "Invalid number of arguments, 3 arguments needed  (%s given)" 
                % len(args)
                )
        
        self.setCenter(args[0], args[1])
        self.setRadius(args[2])
        self.execute()

    def setCenter(self, x, y):
        """Set the center of the circle."""
        self._center.setValue(x, y)
        
    def setRadius(self, radius):
        """Set the radius of the circle."""
        self._radius.setValue(radius)
        
    def center(self):
        """Return the center attribute of the circle."""
        return self._center

    def radius(self):
        """Return the radius value.
        
        :return: radius
        :rtype: double
        """
        return self._radius.value()

    def result(self):
        """Return the cicular line attribute."""
        return self._feature.lastResult()
