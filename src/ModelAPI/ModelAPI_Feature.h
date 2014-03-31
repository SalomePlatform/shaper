// File:        ModelAPI_Feature.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Feature_HeaderFile
#define ModelAPI_Feature_HeaderFile

#include "ModelAPI.h"
#include <string>

class ModelAPI_Feature;

/**\class ModelAPI_Feature
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class MODELAPI_EXPORT ModelAPI_Feature
{
public:
  /// Returns the kind of a feature (like "Point")
  virtual std::string GetKind() = 0;

protected:
  /// Use plugin manager for features creation: this method is 
  /// defined here only for SWIG-wrapping
  ModelAPI_Feature()
  {
  }
};

#endif
