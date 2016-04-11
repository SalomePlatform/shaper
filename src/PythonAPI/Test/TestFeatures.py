import unittest

import ModelAPI

import model

#-----------------------------------------------------------------------------
# Fixtures

class FeaturesFixture(unittest.TestCase):

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

class FeaturesTestCase(FeaturesFixture):

    def test_assert_error_on_empty_args(self):
        features = [
            "addPoint", "addAxis", "addPlane",
            "addImport", "exportToFile",
            "addAddition", "addSubtraction", "addIntersection",
            "addExtrusion",
            # "addExtrusionCut", "addExtrusionFuse",
            "addRevolution",
            # "addRevolutionCut", "addRevolutionFuse",
            "addPlacement", "addRotation", "addTranslation",
            "addGroup",
            "addParameter",
            ]
        for name in features:
            try:
                with self.assertRaises(AssertionError):
                    feature = getattr(model, name)(self.part)
            except AssertionError as e:
                self.fail("%s does not check empty args" % name)

    def test_initialize_without_attributes(self):
        model.construction.axis.Axis(self.part.addFeature("Axis"))
        model.construction.plane.Plane(self.part.addFeature("Plane"))
        model.construction.point.Point(self.part.addFeature("Point"))

        model.exchange.Import(self.part.addFeature("Import"))
        model.exchange.Export(self.part.addFeature("Export"))

        model.features.boolean.Boolean(self.part.addFeature("Boolean"))
        model.features.extrusion.Extrusion(self.part.addFeature("Extrusion"))
        # model.features.extrusion_boolean.ExtrusionBoolean(self.part.addFeature("ExtrusionCut"))
        # model.features.extrusion_boolean.ExtrusionBoolean(self.part.addFeature("ExtrusionFuse"))
        model.features.revolution.Revolution(self.part.addFeature("Revolution"))
        # model.features.revolution_boolean.RevolutionBoolean(self.part.addFeature("RevolutionCut"))
        # model.features.revolution_boolean.RevolutionBoolean(self.part.addFeature("RevolutionFuse"))
        model.features.placement.Placement(self.part.addFeature("Placement"))
        model.features.rotation.Rotation(self.part.addFeature("Rotation"))
        model.features.translation.Translation(self.part.addFeature("Translation"))
        model.features.group.Group(self.part.addFeature("Group"))

        model.parameter.Parameter(self.part.addFeature("Parameter"))

#-----------------------------------------------------------------------------

if __name__ == "__main__":
    unittest.main()
