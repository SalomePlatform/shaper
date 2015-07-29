// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultCompSolid.h
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef Model_ResultCompSolid_H_
#define Model_ResultCompSolid_H_

#include "Model.h"
#include <ModelAPI_ResultCompSolid.h>

/**\class Model_ResultCompSolid
 * \ingroup DataModel
 * \brief The compsolid (container of body results) result of a feature.
 *
 * Provides a container of shapes that may be displayed in the viewer.
 */
class Model_ResultCompSolid : public ModelAPI_ResultCompSolid
{
public:
  /// All features of this sketch (list of references)
  inline static const std::string& BODIES_ID()
  {
    static const std::string MY_BODIES_ID("Bodies");
    return MY_BODIES_ID;
  }

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultCompSolid();

  /// Request for initialization of data model of the object: adding all attributes
  MODEL_EXPORT virtual void initAttributes();

  /// Adds result to the sketch and to its document
  /// \param theIndex an index of the created body result in the compsolid
  /// The real index in the document of the result is an incremented given index
  /// The reason is that the first index is used for the comp solid result on the data
  virtual std::shared_ptr<ModelAPI_ResultBody> addResult(const int theIndex);

  /// Returns the number of sub-elements
  virtual int numberOfSubs(bool forTree = false) const;

  /// Returns the sub-result by zero-base index
  virtual std::shared_ptr<ModelAPI_ResultBody> subResult(const int theIndex,
                                                         bool forTree = false) const;

  /// Returns the sub-feature unique identifier in this composite feature by zero-base index
  //virtual int subResultId(const int theIndex) const;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  virtual bool isSub(ObjectPtr theObject) const;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after)
  virtual void removeResult(std::shared_ptr<ModelAPI_ResultBody> theResult);

protected:
  /// Makes a body on the given feature
  Model_ResultCompSolid();

  friend class Model_Objects;
};

#endif
