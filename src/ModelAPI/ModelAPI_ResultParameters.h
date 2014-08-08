// File:        ModelAPI_ResultParameters.h
// Created:     07 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_ResultParameters_H_
#define ModelAPI_ResultParameters_H_

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
    { return group(); }

  /// Returns the group identifier of this result
  static std::string group()
    {static std::string MY_GROUP = "Parameters"; return MY_GROUP;}
};

//! Pointer on feature object
typedef boost::shared_ptr<ModelAPI_ResultParameters> ResultParametersPtr;

#endif
