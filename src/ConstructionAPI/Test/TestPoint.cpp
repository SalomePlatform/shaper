#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Events_Loop.h>
#include <Events_Error.h>

#include <ModelAPI_Feature.h>
#include <ModelHighAPI_Double.h>
#include <ConstructionAPI_Point.h>

#include "MockModelAPI_AttributeDouble.h"
#include "MockModelAPI_Data.h"
#include "MockModelAPI_Feature.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRefOfCopy;
using ::testing::Test;

void null_deleter(void *) {}

class MockEvents_Listener : public Events_Listener {
public:
  MOCK_METHOD1(processEvent,
    void(const std::shared_ptr<Events_Message>& theMessage));
};

class ConstructionAPI_Point_Constructor_Test : public Test {
public:
  MockEvents_Listener aErrorListener;
  MockModelAPI_AttributeDouble aMockAttributeDouble;
  MockModelAPI_Data aMockData;
  MockModelAPI_Feature aMockFeature;

  ConstructionAPI_Point_Constructor_Test() {
    ON_CALL(aMockFeature, getKind())
      .WillByDefault(ReturnRefOfCopy(std::string("Point")));

    ON_CALL(aMockFeature, data())
      .WillByDefault(Return(std::shared_ptr<ModelAPI_Data>(&aMockData, &null_deleter)));

    ON_CALL(aMockData, real(_))
      .WillByDefault(Return(std::shared_ptr<ModelAPI_AttributeDouble>(&aMockAttributeDouble, &null_deleter)));

    Events_Loop::loop()->registerListener(&aErrorListener, Events_Error::errorID());
  }

  ~ConstructionAPI_Point_Constructor_Test() {
    Events_Loop::loop()->removeListener(&aErrorListener);
  }

  void testUsingAttributes() {
    EXPECT_CALL(aMockFeature, getKind());

    EXPECT_CALL(aMockFeature, data())
      .Times(3);

    EXPECT_CALL(aMockData, real("x"));
    EXPECT_CALL(aMockData, real("y"));
    EXPECT_CALL(aMockData, real("z"));
  }

};

TEST_F(ConstructionAPI_Point_Constructor_Test, GetEmptyFeature_SendException) {
  FeaturePtr aFeature;

  EXPECT_CALL(aErrorListener, processEvent(_));

  ConstructionAPI_Point aPoint(aFeature);
}

TEST_F(ConstructionAPI_Point_Constructor_Test, GetEmptyFeatureAndValues_SendException) {
  FeaturePtr aFeature;

  EXPECT_CALL(aErrorListener, processEvent(_));

  ConstructionAPI_Point aPoint(aFeature, 10, "20", std::string("x + 30"));
}

TEST_F(ConstructionAPI_Point_Constructor_Test, GetWrongFeature_SendException) {
  FeaturePtr aFeature(&aMockFeature, &null_deleter);

  ON_CALL(aMockFeature, getKind())
    .WillByDefault(ReturnRefOfCopy(std::string("WrongKind")));

  EXPECT_CALL(aMockFeature, getKind());
  EXPECT_CALL(aErrorListener, processEvent(_));

  ConstructionAPI_Point aPoint(aFeature);
}


TEST_F(ConstructionAPI_Point_Constructor_Test, GetFeature) {
  FeaturePtr aFeature(&aMockFeature, &null_deleter);

  testUsingAttributes();

  ConstructionAPI_Point aPoint(aFeature);
}

TEST_F(ConstructionAPI_Point_Constructor_Test, GetFeatureAndValues) {
  FeaturePtr aFeature(&aMockFeature, &null_deleter);

  testUsingAttributes();

  EXPECT_CALL(aMockAttributeDouble, setValue(10));
  EXPECT_CALL(aMockAttributeDouble, setText("20"));
  EXPECT_CALL(aMockAttributeDouble, setText("x + 30"));

  EXPECT_CALL(aMockFeature, execute());

  ConstructionAPI_Point aPoint(aFeature, 10, "20", std::string("x + 30"));
}
