// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultGroup.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultGroup_H_
#define ModelAPI_ResultGroup_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <memory>
#include <string>

/**\class ModelAPI_ResultGroup
 * \ingroup DataModel
 * \brief The groups result.
 *
 * Provides a compound of selected elements, without storage, one the fly.
 */
class ModelAPI_ResultGroup : public ModelAPI_Result
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
    static std::string MY_GROUP = "Groups";
    return MY_GROUP;
  }

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultGroup> ResultGroupPtr;

#endif
