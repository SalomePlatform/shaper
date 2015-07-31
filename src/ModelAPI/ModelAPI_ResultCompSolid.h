// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultCompSolid.hxx
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModelAPI_ResultCompSolid_H_
#define ModelAPI_ResultCompSolid_H_

#include "ModelAPI_Result.h"
#include "ModelAPI_ResultBody.h"
#include <string>

/**\class ModelAPI_ResultCompSolid
* \ingroup DataModel
* \brief The comp solid (container of results) result of a feature.
*
* Provides a conainer of body result that may be displayed in the viewer.
*/
class ModelAPI_ResultCompSolid : public ModelAPI_ResultBody
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_ResultCompSolid();
  /// Returns the group identifier of this result

  /// Returns the feature is disabled or not.
  MODELAPI_EXPORT virtual bool isDisabled() const;

  /// Adds result to the sketch and to its document
  /// \param theIndex an index of the created body result in the compsolid
  virtual std::shared_ptr<ModelAPI_ResultBody> addResult(const int theIndex) = 0;

  /// Returns the number of sub-elements
  virtual int numberOfSubs(bool forTree = false) const = 0;

  /// Returns the sub-result by zero-base index
  virtual std::shared_ptr<ModelAPI_ResultBody> subResult(const int theIndex,
                                                         bool forTree = false) const = 0;

  /// Returns the sub-feature unique identifier in this composite feature by zero-base index
  //virtual int subResultId(const int theIndex) const = 0;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  virtual bool isSub(ObjectPtr theObject) const = 0;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after)
  virtual void removeResult(std::shared_ptr<ModelAPI_ResultBody> theResult) = 0;

protected:
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultCompSolid> ResultCompSolidPtr;

#endif
