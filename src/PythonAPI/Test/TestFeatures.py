import unittest

import ModelAPI

from salome.shaper import model

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
        # assert(model.checkPythonDump())
        # This test checks creation of High API classes from low-level.
        # It does not set any attributes, so features invalid, and dump also invalid.
        model.reset()

#-----------------------------------------------------------------------------
# TestCases

class FeaturesTestCase(FeaturesFixture):

    # def test_assert_error_on_empty_args(self):
    #     features = [
    #         # Implemented in C++, add* without arguments doesn't exist
    #         # "addPoint", "addPlane",
    #         # "addImport", "exportToFile",
    #
    #         "addAxis",
    #         "addCut", "addFuse", "addCommon",
    #         "addExtrusion",
    #         # "addExtrusionCut", "addExtrusionFuse",
    #         "addRevolution",
    #         # "addRevolutionCut", "addRevolutionFuse",
    #         "addPlacement", "addRotation", "addTranslation",
    #         "addGroup",
    #         "addParameter",
    #         ]
    #     for name in features:
    #         try:
    #             with self.assertRaises(AssertionError):
    #                 feature = getattr(model, name)(self.part)
    #         except AssertionError as e:
    #             self.fail("%s does not check empty args" % name)

    def test_addPoint(self):
        model.addPoint(self.part, 10, "20", "x + 30")

    def test_initialize_without_attributes(self):
        import ConstructionAPI
        ConstructionAPI.ConstructionAPI_Axis(self.part.addFeature("Axis"))
        ConstructionAPI.ConstructionAPI_Plane(self.part.addFeature("Plane"))
        ConstructionAPI.ConstructionAPI_Point(self.part.addFeature("Point"))

        import ExchangeAPI
        ExchangeAPI.ExchangeAPI_Import(self.part.addFeature("Import"))

        import FeaturesAPI
        FeaturesAPI.FeaturesAPI_Boolean(self.part.addFeature("Boolean"))
        FeaturesAPI.FeaturesAPI_Extrusion(self.part.addFeature("Extrusion"))
        FeaturesAPI.FeaturesAPI_ExtrusionCut(self.part.addFeature("ExtrusionCut"))
        FeaturesAPI.FeaturesAPI_ExtrusionFuse(self.part.addFeature("ExtrusionFuse"))
        FeaturesAPI.FeaturesAPI_Revolution(self.part.addFeature("Revolution"))
        FeaturesAPI.FeaturesAPI_RevolutionCut(self.part.addFeature("RevolutionCut"))
        FeaturesAPI.FeaturesAPI_RevolutionFuse(self.part.addFeature("RevolutionFuse"))
        FeaturesAPI.FeaturesAPI_Placement(self.part.addFeature("Placement"))
        FeaturesAPI.FeaturesAPI_Rotation(self.part.addFeature("Rotation"))
        FeaturesAPI.FeaturesAPI_Translation(self.part.addFeature("Translation"))

        import CollectionAPI
        CollectionAPI.CollectionAPI_Group(self.part.addFeature("Group"))

        import PrimitivesAPI
        PrimitivesAPI.PrimitivesAPI_Box(self.part.addFeature("Box"))

        import ParametersAPI
        ParametersAPI.ParametersAPI_Parameter(self.part.addFeature("Parameter"))

#-----------------------------------------------------------------------------

if __name__ == "__main__":
    unittest.main()
