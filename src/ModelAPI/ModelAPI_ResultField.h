// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultField.hxx
// Created:     16 Nov 2016
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultField_H_
#define ModelAPI_ResultField_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <memory>
#include <string>

/**\class ModelAPI_ResultField
 * \ingroup DataModel
 * \brief The fields result.
 *
 * Provides a compound of selected elements, without storage, one the fly.
 */
class ModelAPI_ResultField : public ModelAPI_Result
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_ResultField();
  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Fields";
    return MY_GROUP;
  }

  /// default color for a result body
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_GROUP_COLOR("150,150,180");
    return RESULT_GROUP_COLOR;
  }

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultField> ResultFieldPtr;

#endif
