from GeomDataAPI import geomDataAPI_Point2D

class Line():
    """Interface for editing of a sketch line feature."""
    def __init__(self, line_feature, *args):
        self._feature = line_feature
        self._start_point = geomDataAPI_Point2D(
            self._feature.data().attribute("StartPoint")
            )
        self._end_point = geomDataAPI_Point2D(
            self._feature.data().attribute("EndPoint")
            )
        if len(args) == 4:
            self.__createByCoordinates(*args)
        elif len(args) == 2:
            self.__createByPoints(*args)
        elif len(args) == 1:
            self.__createByName(sketch, *args)
        else:
            raise Exception("cannot create the Line")

    def __createByCoordinates(self, x1, y1, x2, y2):
        self._start_point.setValue(x1, y1)
        self._end_point.setValue(x2, y2)
        self._feature.execute()

    def __createByPoints(self, p1, p2):
        self._start_point.setValue(p1.x(), p1.y())
        self._end_point.setValue(p2.x(), p2.y())
        self._feature.execute()

    def __createByName(self, sketch, name):
        self._feature.data().selection("External").selectSubShape("EDGE", name)
        self._feature.execute()
        rigid = sketch.addFeature("SketchConstraintRigid")
        rigid.refattr("ConstraintEntityA").setObject(self._feature.firstResult())

    def startPointData(self):
        return self._start_point

    def endPointData(self):
        return self._end_point

    def result(self):
        return self._feature.firstResult()