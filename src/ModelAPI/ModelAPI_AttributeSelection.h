// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_AttributeSelection_H_
#define ModelAPI_AttributeSelection_H_

#include "ModelAPI_Attribute.h"
#include <ModelAPI_Result.h>

class GeomAPI_Edge;
class GeomAPI_Pnt;

/**\class ModelAPI_AttributeSelection
 * \ingroup DataModel
 * \brief Attribute that contains reference to the sub-shape of some result, the selected shape.
 */

class ModelAPI_AttributeSelection : public ModelAPI_Attribute
{
 public:
   /// Type of the center of the circular of elliptic edge
   enum CenterType {
     NOT_CENTER, ///< this is not a center
     CIRCLE_CENTER, ///< center of the circle
     ELLIPSE_FIRST_FOCUS, ///< first focus point of the ellipse
     ELLIPSE_SECOND_FOCUS, ///< second focus point of the ellipse
   };

  /// Defines the result and its selected sub-shape
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, do not store and name the added in the data framework
  ///           (used to remove immediately, without the following updates)
  /// \returns true if attribute was updated
  virtual bool setValue(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily = false) = 0;

  /// Same as SetValue, but it takes an edge (on circular or elliptic curve)
  /// and stores the vertex of the central point (for ellipse the first or the second focus point)
  virtual void setValueCenter(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Edge>& theEdge,
    const CenterType theCenterType,
    const bool theTemporarily = false) = 0;

  /// Makes this selection attribute selects the same as in theSource selection
  virtual void selectValue(
    const std::shared_ptr<ModelAPI_AttributeSelection>& theSource) = 0;

  /// Reset temporary stored values
  virtual void removeTemporaryValues() = 0;

  /// Returns the selected subshape
  virtual std::shared_ptr<GeomAPI_Shape> value() = 0;

  /// Returns the context of the selection (the whole shape owner)
  virtual ResultPtr context() = 0;

  /// Returns the context of the selection if the whole feature was selected
  virtual std::shared_ptr<ModelAPI_Feature> contextFeature() = 0;
  /// Returns the context of the selection : result or feature
  virtual std::shared_ptr<ModelAPI_Object> contextObject() = 0;

  /// Updates the selection due to the changes in the referenced objects
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
  /// \param theDefaultValue a value, which is used if the naming name can not be obtained
  virtual std::wstring namingName(const std::wstring& theDefaultValue = L"") = 0;

  /// Defines an id of the selection
  virtual void setId(int theID) = 0;

  /// Selects sub-shape by the textual Name
  virtual void selectSubShape(const std::string& theType, const std::wstring& theSubShapeName) = 0;

  /// Selects sub-shape by its inner point
  virtual void selectSubShape(const std::string& theType,
                              const std::shared_ptr<GeomAPI_Pnt>& thePoint) = 0;

  /// Selects sub-shape by weak naming index
  virtual void selectSubShape(const std::string& theType,
    const std::wstring& theContextName, const int theIndex) = 0;

  /// Returns true if recompute of selection become impossible
  virtual bool isInvalid() = 0;

  /// Returns true if is geometrical selection.
  virtual bool isGeometricalSelection() const = 0;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeSelection();

  /// Returns the name by context. Adds the part name if the context is located in other document
  MODELAPI_EXPORT virtual std::wstring contextName(const ResultPtr& theContext) const = 0;

  /// Makes the current local selection becomes all sub-shapes with same base geometry.
  MODELAPI_EXPORT virtual void combineGeometrical() = 0;

  /// Updates the arguments of selection if something was affected by creation
  /// or reorder of features upper in the history line.
  /// Returns theRemove true if this attribute must be removed (become deleted)
  MODELAPI_EXPORT virtual void updateInHistory(bool& theRemove) = 0;

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeSelection();
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeSelection> AttributeSelectionPtr;

#endif
