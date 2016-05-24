// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MockModelAPI_Object_H_
#define MockModelAPI_Object_H_

#include <gmock/gmock.h>

#include <ModelAPI_Object.h>

class MockModelAPI_Object : public ModelAPI_Object {
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
      virtual void MODELAPI_EXPORT());
};

#endif // MockModelAPI_Object_H_
