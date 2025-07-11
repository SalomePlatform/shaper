// Copyright (C) 2014-2025  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef Model_AttributeSelectionList_H_
#define Model_AttributeSelectionList_H_

#include "Model.h"
#include "Model_AttributeSelection.h"
#include <ModelAPI_AttributeSelectionList.h>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <vector>
#include <map>

/**\class Model_AttributeSelectionList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to the sub-shapes with
 * possibility to manage them.
 */

class Model_AttributeSelectionList : public ModelAPI_AttributeSelectionList
{
  TDF_Label myLab; ///< the main label of this attribute
  Handle(TDataStd_Integer) mySize;  ///< Contains size of this list
  /// Contains current type name (same as selection attribute)
  Handle(TDataStd_Comment) mySelectionType;
  std::shared_ptr<Model_AttributeSelection> myTmpAttr; ///< temporary attribute (the last one)
  /// the cashed shapes to optimize isInList method: from context to set of shapes in this context
  std::map<ObjectPtr, std::list<std::shared_ptr<GeomAPI_Shape> > > myCash;
  bool myIsCashed; ///< true if cashing is performed
public:
  /// Adds the new reference to the end of the list
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, do not store and name the added in the data framework
  ///           (used to remove immediately, without the following updates)
  MODEL_EXPORT virtual void append(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily = false);

  /// Adds the new reference to the end of the list by the naming name of the selected shape
  /// The type of shape is taken from the current selection type if the given is empty
  MODEL_EXPORT virtual void append(const std::wstring& theNamingName,
                                   const std::string& theType="");

  /// Adds the new reference to the end of the list by inner point on the selected shape
  MODEL_EXPORT virtual void append(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                                   const std::string& theType);

  /// Adds the new reference to the end of the list by weak naming index
  MODEL_EXPORT virtual void append(const std::string& theType,
                                   const std::wstring& theContextName,
                                   const int theIndex);

  /// Copy the selection list to the destination attribute
  MODEL_EXPORT virtual void copyTo(AttributeSelectionListPtr theTarget) const;

  /// Reset temporary stored values
  virtual void removeTemporaryValues();

  /// Removes the last element in the list
  MODEL_EXPORT virtual void removeLast();

  /// Removes the elements from the list.
  /// \param theIndices a list of indices of elements to be removed
  MODEL_EXPORT virtual void remove(const std::set<int>& theIndices);

  /// Returns the number of selection attributes in the list
  MODEL_EXPORT virtual int size();

  /// Returns true if the object with the shape are in list
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, it checks also the temporary added item
  /// \returns true if the pair is found in the attribute
  MODEL_EXPORT virtual bool isInList(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily = false);

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

  /// Starts or stops cashing of the values in the attribute (the cash may become invalid
  /// on modification of the attribute or sub-elements, so the cash must be enabled only
  /// during non-modification operations with this attribute)
  MODEL_EXPORT virtual void cashValues(const bool theEnabled);

  MODEL_EXPORT virtual void setGeometricalSelection(const bool theIsGeometricalSelection) override;

  /// Returns true if is geometrical selection.
  MODEL_EXPORT virtual bool isGeometricalSelection() const override;

  /// Returns a selection filters feature if it is defined for this selection list
  MODEL_EXPORT virtual FiltersFeaturePtr filters() const;

  /// Sets a selection filters feature if it is defined for this selection list
  MODEL_EXPORT virtual void setFilters(FiltersFeaturePtr theFeature);

  /// Applies the filters to this selection list
  MODEL_EXPORT virtual void applyFilters();

  /// Removes the filters of this selection list
  MODEL_EXPORT virtual void removeFilters();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeSelectionList(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  /// Tries to merge attributes in this list with the same result shape. Returns true
  /// if theStart matches with some later attribute and theStart is removed from the list.
  bool merge(Model_AttributeSelection* theStart);

  friend class Model_Data;
  friend class Model_AttributeSelection;
};

#endif
