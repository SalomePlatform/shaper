// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeSelection.h
// Created:     2 Oct 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeSelection_H_
#define ModelAPI_AttributeSelection_H_

#include "ModelAPI_Attribute.h"
#include <ModelAPI_Result.h>

/**\class ModelAPI_AttributeSelection
 * \ingroup DataModel
 * \brief Attribute that contains reference to the sub-shape of some result, the selected shape.
 */

class ModelAPI_AttributeSelection : public ModelAPI_Attribute
{
 public:
  /// Defines the result and its selected sub-shape
  virtual void setValue(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape) = 0;

  /// Returns the selected subshape
  virtual std::shared_ptr<GeomAPI_Shape> value() = 0;

  /// Returns the context of the selection (the whole shape owner)
  virtual ResultPtr context() = 0;

  /// Updates the underlied selection due to the changes in the referenced objects
  /// \returns false if update is failed
  virtual bool update() = 0;

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return "Selection";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// Returns a textual string of the selection
  virtual std::string namingName() = 0;
  
  /// Returns an id of the selection
  virtual int Id() = 0;


  /// Selects sub-shape by the textual Name
  virtual void selectSubShape(const std::string& theType, const std::string& theSubShapeName) = 0;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeSelection();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeSelection();
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeSelection> AttributeSelectionPtr;

#endif
