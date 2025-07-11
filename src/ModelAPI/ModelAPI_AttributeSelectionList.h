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

#ifndef ModelAPI_AttributeSelectionList_H_
#define ModelAPI_AttributeSelectionList_H_

#include "ModelAPI_AttributeSelection.h"
#include <ModelAPI_Result.h>
#include <ModelAPI_Filter.h>

class GeomAPI_Pnt;
class GeomAPI_Shape;

/**\class ModelAPI_AttributeSelectionList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to the sub-shapes with
 * possibility to manage them.
 */

class ModelAPI_AttributeSelectionList : public ModelAPI_Attribute
{
  /// Flag that indicates that the whole result selection is allowed while the selection type
  /// may be sub-objects, so, it is the same as all sub-shapes are selected (#3005). It is "false"
  /// by default.
  bool myIsWholeResultAllowed;
  /// Flag that indicates that update in history must check the copy-features
  /// and make a copy of selection for them.
  bool myMakeCopy;
public:
  /// Adds the new reference to the end of the list
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, do not store and name the added in the data framework
  ///           (used to remove immediately, without the following updates)
  virtual void append(const ObjectPtr& theContext,
                      const std::shared_ptr<GeomAPI_Shape>& theSubShape,
                      const bool theTemporarily = false) = 0;

  /// Adds the new reference to the end of the list by the naming name of the selected shape
  /// The type of shape is taken from the current selection type if the given is empty
  virtual void append(const std::wstring& theNamingName, const std::string& theType = "") = 0;

  /// Adds the new reference to the end of the list by inner point on the selected shape
  virtual void append(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                      const std::string& theType) = 0;

  /// Adds the new reference to the end of the list by weak naming index
  virtual void append(const std::string& theType, const std::wstring& theContextName,
                      const int theIndex) = 0;

  /// Copy the selection list to the destination attribute
  virtual void copyTo(std::shared_ptr<ModelAPI_AttributeSelectionList> theTarget) const = 0;

  /// Reset temporary stored values
  virtual void removeTemporaryValues() = 0;

  /// Removes the last element in the list
  virtual void removeLast() = 0;

  /// Removes the elements from the list.
  /// \param theIndices a list of indices of elements to be removed
  virtual void remove(const std::set<int>& theIndices) = 0;

  /// Returns the number of selection attributes in the list
  virtual int size() = 0;

  /// Returns true if the object with the shape are in list
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, it checks also the temporary added item
  /// \returns true if the pair is found in the attribute
  virtual bool isInList(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily = false) = 0;

  /// The type of all elements selection
  /// \returns the index of the enumeration of the type of shape
  virtual const std::string selectionType() const = 0;

  /// Sets the type of all elements selection
  /// \param theType the index of the enumeration of the type of shape
  virtual void setSelectionType(const std::string& theType) = 0;

  /// Returns the attribute selection by the index (zero based)
  virtual std::shared_ptr<ModelAPI_AttributeSelection> value(const int theIndex) = 0;

  /// Removes all attributes of the list.
  virtual void clear() = 0;

  /// Starts or stops cashing of the values in the attribute (the cash may become invalid
  /// on modification of the attribute or sub-elements, so the cash must be enabled only
  /// during non-modification operations with this attribute)
  virtual void cashValues(const bool theEnabled) = 0;

  virtual void setGeometricalSelection(const bool theIsGeometricalSelection) = 0;

  /// Returns true if is geometrical selection.
  virtual bool isGeometricalSelection() const = 0;

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return "SelectionList";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeSelectionList();

  /// Returns a selection filters feature if it is defined for this selection list
  MODELAPI_EXPORT virtual FiltersFeaturePtr filters() const = 0;

  /// Sets a selection filters feature if it is defined for this selection list
  MODELAPI_EXPORT virtual void setFilters(FiltersFeaturePtr theFeature) = 0;

  /// Applies the filters to this selection list
  MODELAPI_EXPORT virtual void applyFilters() = 0;

  /// Removes the filters of this selection list
  MODELAPI_EXPORT virtual void removeFilters() = 0;

  /// Returns true if the whole result selection corresponds to selection of all sub-shapes.
  MODELAPI_EXPORT virtual const bool isWholeResultAllowed() const {
    return myIsWholeResultAllowed;
  }

  /// Sets whether the whole result selection corresponds to selection of all sub-shapes.
  MODELAPI_EXPORT virtual void setWholeResultAllowed(const bool theFlag)  {
    myIsWholeResultAllowed = theFlag;
  }

  /// Returns true if a copy features must be used in update in history.
  MODELAPI_EXPORT virtual const bool isMakeCopy() const {
    return myMakeCopy;
  }

  /// Sets true if a copy features must be used in update in history.
  MODELAPI_EXPORT virtual void setMakeCopy(const bool theFlag)  {
    myMakeCopy = theFlag;
  }

protected:
  /// Default constructor
  MODELAPI_EXPORT ModelAPI_AttributeSelectionList() : ModelAPI_Attribute()
  {myIsWholeResultAllowed = false; myMakeCopy = false;}

};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeSelectionList> AttributeSelectionListPtr;

#endif
