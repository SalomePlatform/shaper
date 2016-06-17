// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MockModelAPI_Feature_H_
#define MockModelAPI_Feature_H_

#include <gmock/gmock.h>

#include <ModelAPI_Feature.h>

class MockModelAPI_Feature : public ModelAPI_Feature {
  // ModelAPI_Object
 public:
  MOCK_METHOD0(groupName,
      std::string());
  MOCK_METHOD0(initAttributes,
      void());
  MOCK_METHOD0(isDisabled,
      bool());
  MOCK_METHOD3(colorConfigInfo,
      void(std::string& theSection, std::string& theName, std::string& theDefault));
  MOCK_METHOD0(init,
      void());
  MOCK_CONST_METHOD0(data,
      std::shared_ptr<ModelAPI_Data>());

  // ModelAPI_Feature
 public:
  MOCK_METHOD0(getKind,
      const std::string&());
  MOCK_CONST_METHOD0(document,
      std::shared_ptr<ModelAPI_Document>());
  MOCK_METHOD0(execute,
      void());
  MOCK_METHOD1(compute,
      bool(const std::string& theAttributeId));
  MOCK_METHOD2(setError,
      void(std::string, bool));
  MOCK_CONST_METHOD0(error,
      std::string());
  MOCK_METHOD0(isPersistentResult,
      bool());
  MOCK_METHOD0(isAction,
      bool());
};

#endif // MockModelAPI_Feature_H_
