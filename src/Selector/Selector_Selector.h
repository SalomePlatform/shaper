// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Selector_Selector_H_
#define Selector_Selector_H_

#include "Selector.h"

#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TopoDS_Shape.hxx>
#include <list>

class Selector_NameGenerator;

/**\class Selector_Selector
 * \ingroup DataModel
 * \brief Main object for selection of the sub-shapes in the parametrically updated
 * shapes using topological naming mechanism.
 */
class Selector_Selector
{
  /// Type of a selector: on this type depends what is stored in this label and how to
  /// restore it on update.
  enum Selector_Type {
    SELTYPE_CONTAINER, ///< just a container of sub-elements, keeps the shape type of container
    SELTYPE_INTERSECT, ///< sub-shape is intersection of higher level objects
    SELTYPE_PRIMITIVE, ///< sub-shape found as a primitive on some label
    SELTYPE_MODIFICATION, ///< modification of base shapes to the final label
    SELTYPE_FILTER_BY_NEIGHBOR,  ///< identification by neighbor shapes in context
    SELTYPE_WEAK_NAMING, ///< pure weak naming by weak index in context
  };

  Selector_Type myType; ///< Type of this selector.
  TopAbs_ShapeEnum myShapeType; ///< type of this shape: in container, intersection or neighbors
  std::list<Selector_Selector> mySubSelList; ///< list of sub-selectors if needed
  TDF_Label myFinal; ///< final label of the primitive or generation, where the value is
  TDF_LabelList myBases; ///< initial labels that contain shapes that produce the modification
  int myWeakIndex; ///< index of the shape among commons for the modification type (-1 - not set)

  std::list<int> myNBLevel; ///< list of integers corresponding to mySubSelList neighborhood level

  TDF_Label myLab; ///< main label where selector is performed

  TDF_Label myBaseDocumentLab; ///< an access-label to the document that may contain initial shapes

  bool myGeometricalNaming; ///< flag that indicates that geometrical naming selection is enabled
  bool myAlwaysGeometricalNaming; /// to enable geometrical naming from beginning, at select

public:
  /// Initializes selector on the label
  SELECTOR_EXPORT Selector_Selector(TDF_Label theLab);
  /// Returns label of this selector
  SELECTOR_EXPORT TDF_Label label();

  /// Sets the base document access label.
  SELECTOR_EXPORT void setBaseDocument(const TDF_Label theAccess);

  /// Initializes the selector structure on the label.
  /// Stores the name data to restore after modification.
  /// \param theContext whole shape that contains the selected sub-shape
  /// \param theValue selected subshape
  /// \param theGeometricalNaming treats selection with equal surfaces as one
  /// \param theUseNeighbors enables searching algorithm by neighbors
  /// \param theUseIntersections enables searching algorithm by intersection of higher level shapes
  SELECTOR_EXPORT bool select(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
    const bool theGeometricalNaming = false,
    const bool theUseNeighbors = true, const bool theUseIntersections = true);

  /// Stores the name to the label and sub-labels tree
  SELECTOR_EXPORT void store();

  /// Restores the selected shape by the topological naming kept in the data structure
  /// Returns true if it can restore structure correctly
  SELECTOR_EXPORT bool restore();

  /// Restores the selected shape by the topological name string.
  /// Returns not empty label of the context.
  SELECTOR_EXPORT TDF_Label restoreByName(
    std::string theName, const TopAbs_ShapeEnum theShapeType,
    Selector_NameGenerator* theNameGenerator, const bool theGeometricalNaming = false);

  /// Updates the current shape by the stored topological name
  SELECTOR_EXPORT bool solve(const TopoDS_Shape& theContext);

  /// Returns the current sub-shape value (null if can not resolve)
  SELECTOR_EXPORT TopoDS_Shape value();

  /// Returns the naming name of the selection
  SELECTOR_EXPORT std::string name(Selector_NameGenerator* theNameGenerator);

  /// Makes the current local selection becomes all sub-shapes with same base geometry.
  SELECTOR_EXPORT void combineGeometrical(const TopoDS_Shape theContext);

private:

  /// Create and keep in the list the sub-selector that select the given value.
  /// Returns true if selection is correct.
  bool selectBySubSelector(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
    const bool theGeometricalNaming = false, const bool theUseNeighbors = true,
    const bool theUseIntersections = true);
  /// Searches the final shapes presented in all results from bases basing on modification fields
  void findModificationResult(TopoDS_ListOfShape& theCommon);
};

#endif
