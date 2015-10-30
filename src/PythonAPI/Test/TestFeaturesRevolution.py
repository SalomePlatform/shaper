import unittest

import ModelAPI

import model
from model.tools import Selection

#-----------------------------------------------------------------------------
# Fixtures

class FeaturesAddRevolutionFixture(unittest.TestCase):

    def setUp(self):
        model.begin()
        # Create part
        partset = model.moduleDocument()
        self.part = model.addPart(partset).document()
        model.do()

    def tearDown(self):
        model.end()
        model.reset()


class FeaturesRevolutionFixture(FeaturesAddRevolutionFixture):

    def setUp(self):
        FeaturesAddRevolutionFixture.setUp(self)
        # Create revolution
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = base_sketch.addCircle(0, 0, 10)
        # axis
        axis_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        line = axis_sketch.addLine(20, -10, 20, 10)

        model.do()

        base = base_sketch.selectFace(circle.lastResult())
        axis_object = Selection(axis_sketch.firstResult(),
                                line.firstResult().shape())

        self.revolution = model.addRevolution(self.part, base, axis_object,
                                              0, 180)

        model.do()

    def tearDown(self):
        FeaturesAddRevolutionFixture.tearDown(self)

#-----------------------------------------------------------------------------
# TestCases

class FeaturesAddRevolutionTestCase(FeaturesAddRevolutionFixture):

    def test_add_revolution_no_base(self):
        try:
            revolution = model.addRevolution(self.part)
            fail("addRevolution should assert if base is not None")
        except AssertionError:
            pass

    def test_add_revolution_by_face_and_angles(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = base_sketch.addCircle(0, 0, 10)
        # axis
        axis_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        line = axis_sketch.addLine(20, -10, 20, 10)

        model.do()

        base = base_sketch.selectFace(circle.lastResult())
        axis_object = Selection(axis_sketch.firstResult(),
                                line.firstResult().shape())

        revolution = model.addRevolution(self.part, base, axis_object,
                                         0, 180)

        self.assertEqual(revolution.getCreationMethod().value(), "ByAngles")
        self.assertEqual(revolution.getToAngle().value(), 0)
        self.assertEqual(revolution.getFromAngle().value(), 180)
        self.assertEqual(revolution.getToObject().context(), None)
        self.assertEqual(revolution.getToOffset().value(), 0)
        self.assertEqual(revolution.getFromObject().context(), None)
        self.assertEqual(revolution.getFromOffset().value(), 0)

    def test_add_revolution_by_face_and_planes(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        base_circle = base_sketch.addCircle(0, 0, 10)
        # axis
        axis_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        line = axis_sketch.addLine(20, -10, 20, 10)
        # to
        to_plane = model.defaultPlane("XOY")
        to_plane.location().setZ(10)
        to_sketch = model.addSketch(self.part, to_plane)
        to_circle = to_sketch.addCircle(0, 0, 10)
        # from
        from_plane = model.defaultPlane("XOY")
        from_plane.location().setZ(-10)
        from_sketch = model.addSketch(self.part, from_plane)
        from_circle = from_sketch.addCircle(0, 0, 10)

        model.do()

        base = base_sketch.selectFace(base_circle.result())
        axis_object = Selection(axis_sketch.firstResult(),
                                line.firstResult().shape())
        to_obejct = to_sketch.selectFace(to_circle.result())[0]
        from_object = from_sketch.selectFace(from_circle.result())[0]

        revolution = model.addRevolution(self.part, base, axis_object,
                                         to_obejct, 15,
                                         from_object, 20)

        self.assertEqual(revolution.getCreationMethod().value(), "ByPlanesAndOffsets")
        self.assertEqual(revolution.getToAngle().value(), 0)
        self.assertEqual(revolution.getFromAngle().value(), 0)
#         self.assertEqual(revolution.getToObject().context(),
#                          to_sketch.result())
        self.assertEqual(revolution.getToOffset().value(), 15)
#         self.assertEqual(revolution.getFromObject().context(),
#                          from_sketch.result())
        self.assertEqual(revolution.getFromOffset().value(), 20)


class FeaturesRevolutionTestCase(FeaturesRevolutionFixture):

    def test_revolution_feature_calls(self):
        # call method of the feature
        self.assertEqual(self.revolution.getKind(), "Revolution")

    def test_revolution_get_attribute(self):
        # call method of the feature
        self.assertTrue(isinstance(self.revolution.getBase(),
                                   ModelAPI.ModelAPI_AttributeSelectionList))
        self.assertTrue(isinstance(self.revolution.getAxisObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.revolution.getCreationMethod(),
                                   ModelAPI.ModelAPI_AttributeString))
        self.assertTrue(isinstance(self.revolution.getToAngle(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.revolution.getFromAngle(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.revolution.getToObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.revolution.getToOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.revolution.getFromObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.revolution.getFromOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))

    def test_revolution_set_angles(self):
        self.revolution.setAngles(90, 270)
        self.assertEqual(self.revolution.getCreationMethod().value(), "ByAngles")
        self.assertEqual(self.revolution.getToAngle().value(), 90)
        self.assertEqual(self.revolution.getFromAngle().value(), 270)
        self.assertEqual(self.revolution.getToObject().context(), None)
        self.assertEqual(self.revolution.getToOffset().value(), 0)
        self.assertEqual(self.revolution.getFromObject().context(), None)
        self.assertEqual(self.revolution.getFromOffset().value(), 0)

    def test_revolution_set_planes_and_offsets(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        base_circle = base_sketch.addCircle(0, 0, 10)
        # axis
        axis_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        line = axis_sketch.addLine(20, -10, 20, 10)
        # to
        to_plane = model.defaultPlane("XOY")
        to_plane.location().setZ(10)
        to_sketch = model.addSketch(self.part, to_plane)
        to_circle = to_sketch.addCircle(0, 0, 10)
        # from
        from_plane = model.defaultPlane("XOY")
        from_plane.location().setZ(-10)
        from_sketch = model.addSketch(self.part, from_plane)
        from_circle = from_sketch.addCircle(0, 0, 10)

        model.do()

        base = base_sketch.selectFace(base_circle.result())
        axis_object = Selection(axis_sketch.firstResult(),
                                line.firstResult().shape())
        to_obejct = to_sketch.selectFace(to_circle.result())[0]
        from_object = from_sketch.selectFace(from_circle.result())[0]

        self.revolution.setPlanesAndOffsets(to_obejct, 15, from_object, 20)

        self.assertEqual(self.revolution.getCreationMethod().value(), "ByPlanesAndOffsets")
        self.assertEqual(self.revolution.getToAngle().value(), 0)
        self.assertEqual(self.revolution.getFromAngle().value(), 0)
#         self.assertEqual(self.revolution.getToObject().context(), None)
        self.assertEqual(self.revolution.getToOffset().value(), 15)
#         self.assertEqual(self.revolution.getFromObject().context(), None)
        self.assertEqual(self.revolution.getFromOffset().value(), 20)


if __name__ == "__main__":
    unittest.main()
