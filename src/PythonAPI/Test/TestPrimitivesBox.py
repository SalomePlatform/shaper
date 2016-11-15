import unittest

import ModelAPI

import model

class PrimitivesAddBox(unittest.TestCase):

    def setUp(self):
        model.begin()
        # Create part
        partset = model.moduleDocument()
        self.part = model.addPart(partset).document()
        model.do()

    def tearDown(self):
        model.end()
        model.reset()
        
#-----------------------------------------------------------------------------
# TestCases

class PrimitivesAddBoxTestCase(PrimitivesAddBox):

    def test_add_box_by_dimensions(self):
        box = model.addBox(self.part, 50, 20, 10)
        model.do()
        self.assertEqual(box.creationMethod().value(),"BoxByDimensions")
        self.assertEqual(box.dx().value(),50)
        self.assertEqual(box.dy().value(),20)
        self.assertEqual(box.dz().value(),10)

    def test_add_box_by_two_points(self):
        point1 = model.addPoint(self.part,0,0,0).result()
        point2 = model.addPoint(self.part,10,10,10).result()
        box = model.addBox(self.part, point1, point2)
        model.do()
        self.assertEqual(box.creationMethod().value(),"BoxByTwoPoints")
        self.assertEqual(box.firstPoint().context().shape().isVertex(),True)
        self.assertEqual(box.secondPoint().context().shape().isVertex(),True)
        
if __name__ == "__main__":
    unittest.main()
