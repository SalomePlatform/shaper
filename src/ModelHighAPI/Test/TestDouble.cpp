#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ModelHighAPI_Double.h>

#include <MockModelAPI_AttributeDouble.h>

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRefOfCopy;
using ::testing::Test;

// TODO(spo): should be common function
void null_deleter(void *) {}

class HighModelAPI_Double_Test : public Test {
public:
  MockModelAPI_AttributeDouble aMockAttributeDouble;
  std::shared_ptr<ModelAPI_AttributeDouble> anAttributeDouble;

  HighModelAPI_Double_Test() {
    anAttributeDouble = std::shared_ptr<ModelAPI_AttributeDouble>(&aMockAttributeDouble, &null_deleter);
  }

  ~HighModelAPI_Double_Test() {
  }
};

TEST_F(HighModelAPI_Double_Test, Default) {
  ModelHighAPI_Double aValue;

  EXPECT_CALL(aMockAttributeDouble, setValue(0.));

  aValue.fillAttribute(anAttributeDouble);
}

TEST_F(HighModelAPI_Double_Test, Double) {
  ModelHighAPI_Double aValue(100);

  EXPECT_CALL(aMockAttributeDouble, setValue(100.));

  aValue.fillAttribute(anAttributeDouble);
}

TEST_F(HighModelAPI_Double_Test, Char) {
  ModelHighAPI_Double aValue("20");

  EXPECT_CALL(aMockAttributeDouble, setText("20"));

  aValue.fillAttribute(anAttributeDouble);
}

TEST_F(HighModelAPI_Double_Test, String) {
  ModelHighAPI_Double aValue(std::string("x + 30"));

  EXPECT_CALL(aMockAttributeDouble, setText("x + 30"));

  aValue.fillAttribute(anAttributeDouble);
}
