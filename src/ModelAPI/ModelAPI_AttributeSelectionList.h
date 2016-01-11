// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, do not store and name the added in the data framework
  ///           (used to remove immediately, without the following updates)
  virtual void append(const ResultPtr& theContext,
                      const GeomShapePtr& theSubShape,
                      const bool theTemporarily = false) = 0;

  /// Adds the new reference to the end of the list by the naming name of the selected shape
  /// The type of shape is taken from the current selection type if the given is empty
  virtual void append(const std::string theNamingName, const std::string& theType = "") = 0;

  /// Removes the last element in the list
  virtual void removeLast() = 0;

  /// Removes the elements from the list.
  /// \param theIndices a list of indices of elements to be removed
  virtual void remove(const std::set<int>& theIndices) = 0;

  /// Returns the number of selection attributes in the list
  virtual int size() = 0;

  /// The type of all elements selection
  /// \returns the index of the enumeration of the type of shape
  virtual const std::string selectionType() const = 0;

  /// Sets the type of all elements selection
  /// \param theType the index of the enumeration of the type of shape
  virtual void setSelectionType(const std::string& theType) = 0;

  /// Returns the attribute selection by the index (zero based)
  virtual std::shared_ptr<ModelAPI_AttributeSelection> value(const int theIndex) = 0;

  /// Returns all attributes
  virtual void clear() = 0;

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return "SelectionList";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeSelectionList();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeSelectionList();
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeSelectionList> AttributeSelectionListPtr;

#endif
