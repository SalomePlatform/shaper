// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MockModelAPI_AttributeSelection_H_
#define MockModelAPI_AttributeSelection_H_

#include <gmock/gmock.h>

#include <ModelAPI_AttributeSelection.h>

class MockModelAPI_AttributeSelection : public ModelAPI_AttributeSelection {
 public:
  MOCK_METHOD3(setValue,
      void(const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
           const bool theTemporarily));
  MOCK_METHOD0(value,
      std::shared_ptr<GeomAPI_Shape>());
  MOCK_METHOD0(context,
      ResultPtr());
  MOCK_METHOD0(update,
      bool());
  MOCK_METHOD1(namingName,
      std::string(const std::string& theDefaultValue));
  MOCK_METHOD0(Id,
      int());
  MOCK_METHOD2(selectSubShape,
      void(const std::string& theType, const std::string& theSubShapeName));
  MOCK_METHOD0(isInvalid,
      bool());
};

#endif // MockModelAPI_AttributeSelection_H_
