// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultPart.h
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultPart_H_
#define ModelAPI_ResultPart_H_

#include "ModelAPI_Result.h"

#include <string>

/**\class ModelAPI_ResultPart
 * \ingroup DataModel
 * \brief The Part document, result of a creation of new part feature.
 *
 * This result leaves without feature: no parametricity for this element,
 * only add/remove, undo/redo.
 */
class ModelAPI_ResultPart : public ModelAPI_Result
{
 public:
  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Parts";
    return MY_GROUP;
  }

  /// part document reference attribute
  inline static const std::string& DOC_REF()
  {
    static const std::string MY_DOC_REF("PartDocument");
    return MY_DOC_REF;
  }

  /// default color for a result body
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_BODY_COLOR("96,157,255");
    return RESULT_BODY_COLOR;
  }

  /// Returns the part-document of this result
  virtual std::shared_ptr<ModelAPI_Document> partDoc() = 0;

  /// Sets this document as current and if it is not loaded yet, loads it
  virtual void activate() = 0;

  /// Returns true if document is activated (loaded into the memory)
  virtual bool isActivated() = 0;

  /// Returns the name of the shape inside of the part
  virtual std::string nameInPart(const std::shared_ptr<GeomAPI_Shape>& theShape) = 0;
  /// Returns the shape by the name in the part
  virtual std::shared_ptr<GeomAPI_Shape> shapeInPart(const std::string& theName) = 0;
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultPart> ResultPartPtr;

#endif
