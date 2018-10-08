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
    SELTYPE_MODIFICATION,
  };

  Selector_Type myType; ///< Type of this selector.
  TopAbs_ShapeEnum myShapeType; ///< type of this shape: container or result of intersection
  TDF_Label myFinal; ///< final label of the primitive or generation, where the value is
  TDF_LabelList myBases; ///< initial labels that contain shapes that produce the modification

  TDF_Label myLab; ///< main label where selector is performed

 public:
  /// Initializes selector on the label
  Selector_Selector(TDF_Label theLab);
  /// Returns label of this selector
  TDF_Label label();

  /// Initializes the selector structure on the label.
  /// Stores the name data to restore after modification.
  bool Select(const TopoDS_Shape theContext, const TopoDS_Shape theValue);

  /// Stores the name to the label and sub-labels tree
  void Store();

  /// Restores the selected shape by the topological naming kept in the data structure
  //TopoDS_Shape Restore();
};

#endif
