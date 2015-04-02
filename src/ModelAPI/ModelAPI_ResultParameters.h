// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultParameters.h
// Created:     07 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef MODELAPI_RESULTPARAMETERS_H_
#define MODELAPI_RESULTPARAMETERS_H_

#include "ModelAPI_Result.h"

/**\class ModelAPI_ResultParameters
 * \ingroup DataModel
 * \brief The construction element result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * Intermediate, light result that in many cases produces a result on the fly.
 */
class ModelAPI_ResultParameters : public ModelAPI_Result
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
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultParameters> ResultParametersPtr;

#endif
