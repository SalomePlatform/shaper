// File:        Model_Iterator.h
// Created:     1 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Iterator_HeaderFile
#define Model_Iterator_HeaderFile

#include "Model.h"
#include "ModelAPI_Iterator.h"
#include <TDF_Label.hxx>
#include <TDF_ChildIDIterator.hxx>

class Model_Document;

/**\class Model_Iterator
 * \ingroup DataModel
 * \brief Allows to iterate features of the document. Is created by the document
 * (see method featuresIterator).
 */

class MODEL_EXPORT Model_Iterator : public ModelAPI_Iterator
{
  std::shared_ptr<Model_Document> myDoc; ///< the document of iterated objects
  TDF_ChildIDIterator myIter; ///< iterator of the features-labels
public:
  /// Iterates to the next feature
  virtual void next();
  /// Returns true if the current iteration is valid and next iteration is possible
  virtual bool more();
  /// Returns the currently iterated feature
  virtual std::shared_ptr<ModelAPI_Feature> current();
  /// Returns the kind of the current feature (faster than Current()->getKind())
  virtual std::string currentKind();
  /// Returns the name of the current feature (faster than Current()->getName())
  virtual std::string currentName();
  /// Don't changes the current position of iterator. Not fast: iterates the left items.
  /// \returns number of left iterations
  virtual int numIterationsLeft();

protected:
  /// Initializes iterator
  /// \param theDoc document where the iteration is performed
  /// \param theLab label of the features group to iterate
  Model_Iterator(std::shared_ptr<Model_Document> theDoc, TDF_Label theLab);

  friend class Model_Document;
};

#endif
