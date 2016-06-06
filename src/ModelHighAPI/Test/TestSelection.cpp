#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ModelHighAPI_Selection.h>

#include <MockModelAPI_AttributeSelection.h>

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRefOfCopy;
using ::testing::Test;

// TODO(spo): should be common function
static void null_deleter(void *) {}

class HighModelAPI_Selection_Test : public Test {
public:
  MockModelAPI_AttributeSelection aMockAttributeSelection;
  std::shared_ptr<ModelAPI_AttributeSelection> anAttributeSelection;

  HighModelAPI_Selection_Test() {
    anAttributeSelection = std::shared_ptr<ModelAPI_AttributeSelection>(&aMockAttributeSelection, &null_deleter);
  }

  ~HighModelAPI_Selection_Test() {
  }
};

TEST_F(HighModelAPI_Selection_Test, Default) {
  ModelHighAPI_Selection aValue;

  EXPECT_CALL(aMockAttributeSelection, setValue(_, _, false));

  aValue.fillAttribute(anAttributeSelection);
}

TEST_F(HighModelAPI_Selection_Test, ResultAndSubShape) {
  std::shared_ptr<ModelAPI_Result> aResult;
  std::shared_ptr<GeomAPI_Shape> aShape;
  ModelHighAPI_Selection aValue(aResult, aShape);

  EXPECT_CALL(aMockAttributeSelection, setValue(_, _, false));

  aValue.fillAttribute(anAttributeSelection);
}

TEST_F(HighModelAPI_Selection_Test, TypeAndSubShapeName) {
  ModelHighAPI_Selection aValue("Type", "SubShapeName");

  EXPECT_CALL(aMockAttributeSelection, selectSubShape("Type", "SubShapeName"));

  aValue.fillAttribute(anAttributeSelection);
}
