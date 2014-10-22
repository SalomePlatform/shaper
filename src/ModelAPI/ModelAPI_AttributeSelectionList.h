// File:        ModelAPI_AttributeSelectionList.h
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeSelectionList_H_
#define ModelAPI_AttributeSelectionList_H_

#include "ModelAPI_AttributeSelection.h"
#include <ModelAPI_Result.h>

/**\class ModelAPI_AttributeSelectionList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to the sub-shapes with
 * possibility to manage them.
 */

class ModelAPI_AttributeSelectionList : public ModelAPI_Attribute
{
 public:
  /// Adds the new reference to the end of the list
  virtual void append(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape) = 0;

  /// Returns the number ofselection attributes in the list
  virtual int size() = 0;

  /// Returns the attribute selection by the index (zero based)
  virtual boost::shared_ptr<ModelAPI_AttributeSelection> value(const int theIndex) = 0;

  /// Returns all attributes
  virtual void clear() = 0;

  /// Returns the type of this class of attributes
  static std::string type()
  {
    return "SelectionList";
  }

  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType()
  {
    return type();
  }

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_AttributeSelectionList()
  {
  }

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeSelectionList()
  {
  }
};

#endif
