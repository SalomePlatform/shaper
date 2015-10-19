// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeSelectionList.h
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeSelectionList_H_
#define Model_AttributeSelectionList_H_

#include "Model.h"
#include "Model_AttributeSelection.h"
#include <ModelAPI_AttributeSelectionList.h>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <vector>

/**\class Model_AttributeSelectionList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to the sub-shapes with
 * possibility to manage them.
 */

class Model_AttributeSelectionList : public ModelAPI_AttributeSelectionList
{
  Handle(TDataStd_Integer) mySize;  ///< Contains size of this list
  Handle(TDataStd_Comment) mySelectionType;  ///< Contains current type name (same as selection attribute)
  std::shared_ptr<Model_AttributeSelection> myTmpAttr; ///< temporary attribute (the last one)
public:
  /// Adds the new reference to the end of the list
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, do not store and name the added in the data framework
  ///           (used to remove immideately, without the following updates)
  MODEL_EXPORT virtual void append(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily = false);

  /// Adds the new reference to the end of the list by the naming name of the selected shape
  /// The type of shape is taken from the current selection type if the given is empty
  MODEL_EXPORT virtual void append(const std::string theNamingName, const std::string& theType="");

  /// Removes the last element in the list
  MODEL_EXPORT virtual void removeLast();

  /// Returns the number ofselection attributes in the list
  MODEL_EXPORT virtual int size();

  /// The type of all elements selection
  /// \returns the index of the OCCT enumeration of the type of shape
  MODEL_EXPORT virtual const std::string selectionType() const;

  /// Sets the type of all elements selection
  /// \param theType the index of the OCCT enumeration of the type of shape
  MODEL_EXPORT virtual void setSelectionType(const std::string& theType);

  /// Returns the attribute selection by the index (zero based)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeSelection> value(const int theIndex);

  /// Returns all attributes
  MODEL_EXPORT virtual void clear();

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeSelectionList(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
