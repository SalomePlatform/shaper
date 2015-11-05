import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetTangent(SketcherTestCase):
    def setUp(self):
        SketcherTestCase.setUp(self)
        self.line = self.sketch.addLine(0, 0, 0, 1)
        self.arc = self.sketch.addArc(0, 1, 0, 0, 1, 1)
        self.sketch.setCoincident(
            self.line.endPointData(), self.arc.startPointData()
            )
        
    def test_set_tangent(self):
        self.sketch.setTangent(self.line.result(), self.arc.result())
        model.do()
        # TODO : find a way to check that the constraint as been set
    
    def test_none_type_arguments(self):
        with self.assertRaises(TypeError):
            self.sketch.setTangent(None, self.arc.result())

if __name__ == "__main__":
    unittest.main(verbosity=2)