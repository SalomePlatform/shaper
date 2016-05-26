// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MockModelAPI_AttributeDouble_H_
#define MockModelAPI_AttributeDouble_H_

#include <gmock/gmock.h>

#include <ModelAPI_AttributeDouble.h>

class MockModelAPI_AttributeDouble : public ModelAPI_AttributeDouble {
 public:
  MOCK_METHOD1(setValue,
      void(const double theValue));
  MOCK_METHOD0(value,
      double());
  MOCK_METHOD1(setCalculatedValue,
      void(const double theValue));
  MOCK_METHOD1(setText,
      void(const std::string& theText));
  MOCK_METHOD0(text,
      std::string());
  MOCK_METHOD1(setExpressionInvalid,
      void(const bool theFlag));
  MOCK_METHOD0(expressionInvalid,
      bool());
  MOCK_METHOD1(setExpressionError,
      void(const std::string& theError));
  MOCK_METHOD0(expressionError,
      std::string());
  MOCK_METHOD1(setUsedParameters,
      void(const std::set<std::string>& theUsedParameters));
  MOCK_CONST_METHOD0(usedParameters,
      std::set<std::string>());
};

#endif // MockModelAPI_AttributeDouble_H_
