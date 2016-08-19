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
        assert(model.checkPythonDump())
        model.reset()


class FeaturesExtrusionFixture(FeaturesAddExtrusionFixture):

    def setUp(self):
        FeaturesAddExtrusionFixture.setUp(self)
        # Create extrusion
        sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = sketch.addCircle(0, 0, 10)
        model.do()

        base = sketch.selectFace()
        self.extrusion = model.addExtrusion(self.part, base, 10, 0)
        model.do()

    def tearDown(self):
        FeaturesAddExtrusionFixture.tearDown(self)

#-----------------------------------------------------------------------------
# TestCases

class FeaturesAddExtrusionTestCase(FeaturesAddExtrusionFixture):

    def test_add_extrusion_by_face_and_size(self):
        sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = sketch.addCircle(0, 0, 10)
        model.do()

        base = sketch.selectFace()
        extrusion = model.addExtrusion(self.part, base, 10, 0)
        model.do()

        self.assertEqual(extrusion.creationMethod().value(), "BySizes")
        self.assertEqual(extrusion.toSize().value(), 10)
        self.assertEqual(extrusion.fromSize().value(), 0)
        self.assertEqual(extrusion.toObject().context(), None)
        self.assertEqual(extrusion.toOffset().isInitialized(), False)
        self.assertEqual(extrusion.fromObject().context(), None)
        self.assertEqual(extrusion.fromOffset().isInitialized(), False)

    def test_add_extrusion_by_face_and_planes(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        base_circle = base_sketch.addCircle(0, 0, 10)
        # to
        to_plane = model.defaultPlane("XOY")
        to_plane.origin().setZ(10)
        to_sketch = model.addSketch(self.part, to_plane)
        to_circle = to_sketch.addCircle(0, 0, 10)
        # from
        from_plane = model.defaultPlane("XOY")
        from_plane.origin().setZ(-10)
        from_sketch = model.addSketch(self.part, from_plane)
        from_circle = from_sketch.addCircle(0, 0, 10)

        model.do()

        base = base_sketch.selectFace()
        to_object = to_sketch.selectFace()[0]
        from_object = from_sketch.selectFace()[0]
        extrusion = model.addExtrusion(self.part, base,
                                       to_object, 15,
                                       from_object, 20)

        self.assertEqual(extrusion.creationMethod().value(), "ByPlanesAndOffsets")
        self.assertEqual(extrusion.toSize().isInitialized(), False)
        self.assertEqual(extrusion.fromSize().isInitialized(), False)
#         self.assertEqual(extrusion.getToObject().context(),
#                          to_sketch.result())
        self.assertEqual(extrusion.toOffset().value(), 15)
#         self.assertEqual(extrusion.getFromObject().context(),
#                          from_sketch.result())
        self.assertEqual(extrusion.fromOffset().value(), 20)


class FeaturesExtrusionTestCase(FeaturesExtrusionFixture):

    def test_extrusion_feature_calls(self):
        # call method of the feature
        self.assertEqual(self.extrusion.getKind(), "Extrusion")

    def test_extrusion_get_attribute(self):
        # call method of the feature
        self.assertTrue(isinstance(self.extrusion.baseObjects(),
                                   ModelAPI.ModelAPI_AttributeSelectionList))
        self.assertTrue(isinstance(self.extrusion.creationMethod(),
                                   ModelAPI.ModelAPI_AttributeString))
        self.assertTrue(isinstance(self.extrusion.toSize(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.extrusion.fromSize(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.extrusion.toObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.extrusion.toOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.extrusion.fromObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.extrusion.fromOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))

    def test_extrusion_set_sizes(self):
        self.extrusion.setSizes(15, 20)
        self.assertEqual(self.extrusion.creationMethod().value(), "BySizes")
        self.assertEqual(self.extrusion.toSize().value(), 15)
        self.assertEqual(self.extrusion.fromSize().value(), 20)
        self.assertEqual(self.extrusion.toObject().context(), None)
        self.assertEqual(self.extrusion.toOffset().isInitialized(), False)
        self.assertEqual(self.extrusion.fromObject().context(), None)
        self.assertEqual(self.extrusion.fromOffset().isInitialized(), False)

    def test_extrusion_set_planes_and_offsets(self):
        # to
        to_plane = model.defaultPlane("XOY")
        to_plane.origin().setZ(10)
        to_sketch = model.addSketch(self.part, to_plane)
        to_circle = to_sketch.addCircle(0, 0, 10)
        # from
        from_plane = model.defaultPlane("XOY")
        from_plane.origin().setZ(-10)
        from_sketch = model.addSketch(self.part, from_plane)
        from_circle = from_sketch.addCircle(0, 0, 10)

        model.do()

        to_object = to_sketch.selectFace()[0]
        from_object = from_sketch.selectFace()[0]

        self.part.moveFeature(self.extrusion.feature(), from_sketch.feature())

        self.extrusion.setPlanesAndOffsets(to_object, 15, from_object, 20)


if __name__ == "__main__":
    unittest.main()
