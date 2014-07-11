// File:        ModelAPI_Result.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Result_HeaderFile
#define ModelAPI_Result_HeaderFile

#include "ModelAPI_Object.h"

class ModelAPI_Feature;

/**\class ModelAPI_Result
 * \ingroup DataModel
 * \brief The result of a feature.
 *
 * A generic class that .
 */
class ModelAPI_Result : public ModelAPI_Object
{
public:
  /// Returns the source feature of this result
  //virtual boost::shared_ptr<ModelAPI_Feature> owner() = 0;

  /// Returns the group identifier of this result
  virtual std::string group() = 0;
};

//! Pointer on feature object
typedef boost::shared_ptr<ModelAPI_Result> ResultPtr;

#endif
