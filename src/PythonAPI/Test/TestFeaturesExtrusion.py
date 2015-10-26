import unittest

import ModelAPI

import model

#-----------------------------------------------------------------------------
# Fixtures

class FeaturesAddExtrusionFixture(unittest.TestCase):

    def setUp(self):
        model.begin()
        # Create part
        partset = model.moduleDocument()
        self.part = model.addPart(partset).document()
        model.do()

    def tearDown(self):
        model.end()
        model.reset()


class FeaturesExtrusionFixture(FeaturesAddExtrusionFixture):

    def setUp(self):
        FeaturesAddExtrusionFixture.setUp(self)
        # Create extrusion
        sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = sketch.addCircle(0, 0, 10)
        model.do()

        base = [sketch.selectFace(circle.result())]
        self.extrusion = model.addExtrusion(self.part, base, 10, 0)
        model.do()

    def tearDown(self):
        FeaturesAddExtrusionFixture.tearDown(self)

#-----------------------------------------------------------------------------
# TestCases

class FeaturesAddExtrusionTestCase(FeaturesAddExtrusionFixture):

    def test_add_extrusion_no_base(self):
        try:
            extrusion = model.addExtrusion(self.part)
            fail("addExtrusion should assert if base is not None")
        except AssertionError:
            pass

    def test_add_extrusion_by_face_and_size(self):
        sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = sketch.addCircle(0, 0, 10)
        model.do()

        base = [sketch.selectFace(circle.result())]
        extrusion = model.addExtrusion(self.part, base, 10, 0)
        model.do()

        self.assertEqual(extrusion.getCreationMethod().value(), "BySizes")
        self.assertEqual(extrusion.getToSize().value(), 10)
        self.assertEqual(extrusion.getFromSize().value(), 0)
        self.assertEqual(extrusion.getToObject().context(), None)
        self.assertEqual(extrusion.getToOffset().value(), 0)
        self.assertEqual(extrusion.getFromObject().context(), None)
        self.assertEqual(extrusion.getFromOffset().value(), 0)

    def test_add_extrusion_by_face_and_planes(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        base_circle = base_sketch.addCircle(0, 0, 10)
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

        base = [base_sketch.selectFace(base_circle.result())]
        to_object = to_sketch.selectFace(to_circle.result())
        from_object = from_sketch.selectFace(from_circle.result())
        extrusion = model.addExtrusion(self.part, base,
                                       to_object, 15,
                                       from_object, 20)

        self.assertEqual(extrusion.getCreationMethod().value(), "ByPlanesAndOffsets")
        self.assertEqual(extrusion.getToSize().value(), 0)
        self.assertEqual(extrusion.getFromSize().value(), 0)
#         self.assertEqual(extrusion.getToObject().context(),
#                          to_sketch.result())
        self.assertEqual(extrusion.getToOffset().value(), 15)
#         self.assertEqual(extrusion.getFromObject().context(),
#                          from_sketch.result())
        self.assertEqual(extrusion.getFromOffset().value(), 20)


class FeaturesExtrusionTestCase(FeaturesExtrusionFixture):

    def test_extrusion_feature_calls(self):
        # call method of the feature
        self.assertEqual(self.extrusion.getKind(), "Extrusion")

    def test_extrusion_get_attribute(self):
        # call method of the feature
        self.assertTrue(isinstance(self.extrusion.getBase(),
                                   ModelAPI.ModelAPI_AttributeSelectionList))
        self.assertTrue(isinstance(self.extrusion.getCreationMethod(),
                                   ModelAPI.ModelAPI_AttributeString))
        self.assertTrue(isinstance(self.extrusion.getToSize(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.extrusion.getFromSize(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.extrusion.getToObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.extrusion.getToOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.extrusion.getFromObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.extrusion.getFromOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))

    def test_extrusion_set_sizes(self):
        self.extrusion.setSizes(15, 20)
        self.assertEqual(self.extrusion.getCreationMethod().value(), "BySizes")
        self.assertEqual(self.extrusion.getToSize().value(), 15)
        self.assertEqual(self.extrusion.getFromSize().value(), 20)
        self.assertEqual(self.extrusion.getToObject().context(), None)
        self.assertEqual(self.extrusion.getToOffset().value(), 0)
        self.assertEqual(self.extrusion.getFromObject().context(), None)
        self.assertEqual(self.extrusion.getFromOffset().value(), 0)

    def test_extrusion_set_planes_and_offsets(self):
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

        to_object = to_sketch.selectFace(to_circle.result())
        from_object = from_sketch.selectFace(from_circle.result())
        self.extrusion.setPlanesAndOffsets(to_object, 15, from_object, 20)


if __name__ == "__main__":
    unittest.main()
