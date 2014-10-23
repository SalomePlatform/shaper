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
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape) = 0;

  /// Returns the selected subshape
  virtual boost::shared_ptr<GeomAPI_Shape> value() = 0;

  /// Returns the context of the selection (the whole shape owner)
  virtual ResultPtr context() = 0;

  /// Returns the type of this class of attributes
  static std::string type()
  {
    return "Selection";
  }

  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType()
  {
    return type();
  }

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_AttributeSelection()
  {
  }

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeSelection()
  {
  }
};

//! Pointer on double attribute
typedef boost::shared_ptr<ModelAPI_AttributeSelection> AttributeSelectionPtr;

#endif
