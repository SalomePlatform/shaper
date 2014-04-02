// File:        ModelAPI_Iterator.hxx
// Created:     1 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Iterator_HeaderFile
#define ModelAPI_Iterator_HeaderFile

#include "ModelAPI.h"
#include <string>
#include <memory>

class ModelAPI_Feature;
class ModelAPI_Document;

/**\class ModelAPI_Iterator
 * \ingroup DataModel
 * \brief Allows to iterate features of the document. Is created by the document
 * (see method featuresIterator).
 */

class MODELAPI_EXPORT ModelAPI_Iterator
{
public:
  /// Iterates to the next feature
  virtual void Next() = 0;
  /// Returns true if the current iteration is valid and next iteration is possible
  virtual bool More() = 0;
  /// Returns the currently iterated feature
  virtual std::shared_ptr<ModelAPI_Feature> Current() = 0;
  /// Returns the kind of the current feature (faster than Current()->getKind())
  virtual std::string CurrentKind() = 0;
  /// Returns the name of the current feature (faster than Current()->getName())
  virtual std::string CurrentName() = 0;

protected:
  /// Use plugin manager for features creation: this method is 
  /// defined here only for SWIG-wrapping
  ModelAPI_Iterator()
  {}
};

#endif
