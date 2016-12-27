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

  class ModelAPI_FieldStep : public ModelAPI_Entity
  {
  public:
    virtual ModelAPI_ResultField* field() const = 0;

    virtual int id() const = 0;
  };

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

  /// Returns number of steps
  virtual int stepsSize() const = 0;

  /// Returns a text line by its number
  /// \param theLine a number of line
  virtual std::string textLine(int theLine) const = 0;

  /// Returns step object
  /// \param theId an id of the object
  virtual ModelAPI_FieldStep* step(int theId) const = 0;
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultField> ResultFieldPtr;
typedef std::shared_ptr<ModelAPI_ResultField::ModelAPI_FieldStep> FieldStepPtr;

#endif
