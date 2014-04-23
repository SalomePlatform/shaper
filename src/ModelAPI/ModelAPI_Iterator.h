// File:        ModelAPI_Iterator.hxx
// Created:     1 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Iterator_HeaderFile
#define ModelAPI_Iterator_HeaderFile

#include "ModelAPI.h"
#include <string>
#include <boost/shared_ptr.hpp>

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
  virtual void next() = 0;
  /// Returns true if the current iteration is valid and next iteration is possible
  virtual bool more() = 0;
  /// Returns the currently iterated feature
  virtual boost::shared_ptr<ModelAPI_Feature> current() = 0;
  /// Returns the kind of the current feature (faster than Current()->getKind())
  virtual std::string currentKind() = 0;
  /// Returns the name of the current feature (faster than Current()->getName())
  virtual std::string currentName() = 0;
  /// Don't changes the current position of iterator. Not fast: iterates the left items.
  /// \returns number of left iterations
  virtual int numIterationsLeft() = 0;
  /// Compares the current feature with the given one
  /// \returns true if given feature equals to the current one
  virtual bool isEqual(boost::shared_ptr<ModelAPI_Feature> theFeature) = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Iterator() {}

protected:
  /// Use plugin manager for features creation: this method is 
  /// defined here only for SWIG-wrapping
  ModelAPI_Iterator()
  {}
};

#endif
