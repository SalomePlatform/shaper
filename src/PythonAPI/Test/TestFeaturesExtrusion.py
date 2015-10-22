import unittest
import model
import ModelAPI

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
        self.extrusion = model.addExtrusion(self.part,
                                            sketch.selectFace(circle.result()),
                                            10, 0)
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
        extrusion = model.addExtrusion(self.part,
                                       sketch.selectFace(circle.result()),
                                       10, 0)

    def test_add_extrusion_by_face_and_planes(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        base_circle = base_sketch.addCircle(0, 0, 10)
        base_sketch.selectFace(base_circle.result())
        # to
        to_plane = model.defaultPlane("XOY")
        to_plane.location().setZ(10)
        to_sketch = model.addSketch(self.part, to_plane)
        to_circle = to_sketch.addCircle(0, 0, 10)
        to_sketch.selectFace(to_circle.result())
        # from
        from_plane = model.defaultPlane("XOY")
        from_plane.location().setZ(-10)
        from_sketch = model.addSketch(self.part, from_plane)
        from_circle = from_sketch.addCircle(0, 0, 10)
        from_sketch.selectFace(from_circle.result())

        extrusion = model.addExtrusion(self.part,
                                       base_sketch,
                                       to_sketch, 15,
                                       from_sketch, 20)


class FeaturesExtrusionTestCase(FeaturesExtrusionFixture):

    def test_extrusion_set_sizes(self):
        self.extrusion.setSize(15, 20)

    def test_extrusion_set_planes_and_offsets(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        base_circle = base_sketch.addCircle(0, 0, 10)
        base_sketch.selectFace(base_circle.result())
        # to
        to_plane = model.defaultPlane("XOY")
        to_plane.location().setZ(10)
        to_sketch = model.addSketch(self.part, to_plane)
        to_circle = to_sketch.addCircle(0, 0, 10)
        to_sketch.selectFace(to_circle.result())
        # from
        from_plane = model.defaultPlane("XOY")
        from_plane.location().setZ(-10)
        from_sketch = model.addSketch(self.part, from_plane)
        from_circle = from_sketch.addCircle(0, 0, 10)
        from_sketch.selectFace(from_circle.result())

        self.extrusion.setPlanesAndOffsets(to_sketch, 15, from_sketch, 20)


if __name__ == "__main__":
    unittest.main()
