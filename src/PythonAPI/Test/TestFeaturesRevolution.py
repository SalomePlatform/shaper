## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

import unittest

import ModelAPI

from salome.shaper import model

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
        assert(model.checkPythonDump())
        model.reset()


class FeaturesRevolutionFixture(FeaturesAddRevolutionFixture):

    def setUp(self):
        FeaturesAddRevolutionFixture.setUp(self)
        # Create revolution
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = base_sketch.addCircle(0, 0, 10)

        model.do()

        base = base_sketch.selectFace()
        axis_point1 = model.addPoint(self.part, 20, -10, 0).result()
        axis_point2 = model.addPoint(self.part, 20, 10, 0).result()
        axis_object = model.addAxis(self.part, axis_point1[0], axis_point2[0]).result()

        self.revolution = model.addRevolution(self.part, base, axis_object[0],
                                              0, 180)

        model.do()

    def tearDown(self):
        FeaturesAddRevolutionFixture.tearDown(self)

#-----------------------------------------------------------------------------
# TestCases

class FeaturesAddRevolutionTestCase(FeaturesAddRevolutionFixture):

    def test_add_revolution_by_face_and_angles(self):
        # base
        base_sketch = model.addSketch(self.part, model.defaultPlane("XOY"))
        circle = base_sketch.addCircle(0, 0, 10)

        model.do()

        base = base_sketch.selectFace()
        axis_point1 = model.addPoint(self.part, 20, -10, 0).result()
        axis_point2 = model.addPoint(self.part, 20, 10, 0).result()
        axis_object = model.addAxis(self.part, axis_point1[0], axis_point2[0]).result()

        revolution = model.addRevolution(self.part, base, axis_object[0],
                                         0, 180)

        self.assertEqual(revolution.creationMethod().value(), "ByAngles")
        self.assertEqual(revolution.toAngle().value(), 0)
        self.assertEqual(revolution.fromAngle().value(), 180)
        self.assertEqual(revolution.toObject().context(), None)
        self.assertEqual(revolution.toOffset().value(), 0)
        self.assertEqual(revolution.fromObject().context(), None)
        self.assertEqual(revolution.fromOffset().value(), 0)

    def test_add_revolution_by_face_and_planes(self):
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
        axis_point1 = model.addPoint(self.part, 20, -10, 0).result()
        axis_point2 = model.addPoint(self.part, 20, 10, 0).result()
        axis_object = model.addAxis(self.part, axis_point1[0], axis_point2[0]).result()
        to_obejct = to_sketch.selectFace()[0]
        from_object = from_sketch.selectFace()[0]

        revolution = model.addRevolution(self.part, base, axis_object[0],
                                         to_obejct, 15,
                                         from_object, 20)

        self.assertEqual(revolution.creationMethod().value(), "ByPlanesAndOffsets")
        self.assertEqual(revolution.toAngle().value(), 0)
        self.assertEqual(revolution.fromAngle().value(), 0)
#         self.assertEqual(revolution.getToObject().context(),
#                          to_sketch.result())
        self.assertEqual(revolution.toOffset().value(), 15)
#         self.assertEqual(revolution.getFromObject().context(),
#                          from_sketch.result())
        self.assertEqual(revolution.fromOffset().value(), 20)


class FeaturesRevolutionTestCase(FeaturesRevolutionFixture):

    def test_revolution_feature_calls(self):
        # call method of the feature
        self.assertEqual(self.revolution.getKind(), "Revolution")

    def test_revolution_get_attribute(self):
        # call method of the feature
        self.assertTrue(isinstance(self.revolution.baseObjects(),
                                   ModelAPI.ModelAPI_AttributeSelectionList))
        self.assertTrue(isinstance(self.revolution.axis(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.revolution.creationMethod(),
                                   ModelAPI.ModelAPI_AttributeString))
        self.assertTrue(isinstance(self.revolution.toAngle(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.revolution.fromAngle(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.revolution.toObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.revolution.toOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))
        self.assertTrue(isinstance(self.revolution.fromObject(),
                                   ModelAPI.ModelAPI_AttributeSelection))
        self.assertTrue(isinstance(self.revolution.fromOffset(),
                                   ModelAPI.ModelAPI_AttributeDouble))

    def test_revolution_set_angles(self):
        self.revolution.setAngles(90, 270)
        self.assertEqual(self.revolution.creationMethod().value(), "ByAngles")
        self.assertEqual(self.revolution.toAngle().value(), 90)
        self.assertEqual(self.revolution.fromAngle().value(), 270)
        self.assertEqual(self.revolution.toObject().context(), None)
        self.assertEqual(self.revolution.toOffset().value(), 0)
        self.assertEqual(self.revolution.fromObject().context(), None)
        self.assertEqual(self.revolution.fromOffset().value(), 0)

    def test_revolution_set_planes_and_offsets(self):
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
        axis_point1 = model.addPoint(self.part, 20, -10, 0).result()
        axis_point2 = model.addPoint(self.part, 20, 10, 0).result()
        axis_object = model.addAxis(self.part, axis_point1[0], axis_point2[0]).result()
        to_obejct = to_sketch.selectFace()[0]
        from_object = from_sketch.selectFace()[0]

        self.revolution.setPlanesAndOffsets(to_obejct, 15, from_object, 20)

        self.assertEqual(self.revolution.creationMethod().value(), "ByPlanesAndOffsets")
        # self.assertEqual(self.revolution.toAngle().value(), 0)
        # self.assertEqual(self.revolution.fromAngle().value(), 0)
#         self.assertEqual(self.revolution.getToObject().context(), None)
        self.assertEqual(self.revolution.toOffset().value(), 15)
#         self.assertEqual(self.revolution.getFromObject().context(), None)
        self.assertEqual(self.revolution.fromOffset().value(), 20)


if __name__ == "__main__":
    unittest.main(exit=False)
