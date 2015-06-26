// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultConstruction.h
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultConstruction_H_
#define ModelAPI_ResultConstruction_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_Face.h>

#include <string>

/**\class ModelAPI_ResultConstruction
 * \ingroup DataModel
 * \brief The construction element result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * Intermediate, light result that in many cases produces a result on the fly.
 */
class ModelAPI_ResultConstruction : public ModelAPI_Result
{
 public:
  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Construction";
    return MY_GROUP;
  }

  /// default color for a result construction
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_CONSTRUCTION_COLOR("120,120,120");
    return RESULT_CONSTRUCTION_COLOR;
  }

  /// Sets the result
  virtual void setShape(std::shared_ptr<GeomAPI_Shape> theShape) = 0;

  /// Sets the flag that it must be displayed in history (default is true)
  virtual void setIsInHistory(const bool isInHistory) = 0;

  /// if the construction result may be used as faces, this method returns not zero number of faces
  virtual int facesNum() = 0;
  /// if the construction result may be used as faces, this method returns face by zero based index
  virtual std::shared_ptr<GeomAPI_Face> face(const int theIndex) = 0;

  /// By default object is not infinite.
  virtual bool isInfinite() = 0;
  /// Sets the flag that it is infinite
  virtual void setInfinite(const bool theInfinite) = 0;
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultConstruction> ResultConstructionPtr;

#endif
