// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultParameter.h
// Created:     07 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef MODELAPI_RESULTPARAMETER_H_
#define MODELAPI_RESULTPARAMETER_H_

#include "ModelAPI_Result.h"

/**\class ModelAPI_ResultParameter
 * \ingroup DataModel
 * \brief The construction element result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * Intermediate, light result that in many cases produces a result on the fly.
 */
class ModelAPI_ResultParameter : public ModelAPI_Result
{
 public:
  /// Returns the group identifier of this result
  virtual std::string groupName()
  {
    return group();
  }

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Parameters";
    return MY_GROUP;
  }

  inline static const std::string& VALUE()
  {
    static const std::string MY_VALUE_ID("Value");
    return MY_VALUE_ID;
  }

  inline static const std::string& STATE()
  {
    static const std::string MY_VALUE_ID("State");
    return MY_VALUE_ID;
  }

  virtual void initAttributes() = 0;

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultParameter> ResultParameterPtr;

#endif
