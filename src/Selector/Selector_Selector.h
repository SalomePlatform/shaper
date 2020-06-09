// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef Selector_Selector_H_
#define Selector_Selector_H_

#include "Selector.h"

#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>

class Selector_NameGenerator;
class Selector_Algo;

/**\class Selector_Selector
 * \ingroup DataModel
 * \brief Main object for selection of the sub-shapes in the parametrically updated
 * shapes using topological naming mechanism.
 */
class Selector_Selector
{
  TDF_Label myLab; ///< main label where selector is performed
  TDF_Label myBaseDocumentLab; ///< an access-label to the document that may contain initial shapes
  Selector_Algo* myAlgo; ///< root algorithm of the selector

public:
  /// Initializes selector on the label
  SELECTOR_EXPORT Selector_Selector(TDF_Label theLab, TDF_Label theBaseDocLab = TDF_Label());

  /// Destructor
  SELECTOR_EXPORT ~Selector_Selector();

  /// Initializes the selector structure on the label.
  /// Stores the name data to restore after modification.
  /// \param theContext whole shape that contains the selected sub-shape
  /// \param theValue selected subshape
  /// \param theGeometricalNaming treats selection with equal surfaces as one
  /// \param theUseNeighbors enables searching algorithm by neighbors
  /// \param theUseIntersections enables searching algorithm by intersection of higher level shapes
  SELECTOR_EXPORT bool select(const TopoDS_Shape theContext, const TopoDS_Shape theValue,
    const bool theGeometricalNaming = false);

  /// Stores the name to the label and sub-labels tree
  SELECTOR_EXPORT bool store(const TopoDS_Shape theContext);

  /// Restores the selected shape by the topological naming kept in the data structure
  /// Returns true if it can restore structure correctly
  SELECTOR_EXPORT bool restore(const TopoDS_Shape theContext);

  /// Restores the selected shape by the topological name string.
  /// Returns not empty label of the context.
  SELECTOR_EXPORT TDF_Label restoreByName(
    std::wstring theName, const TopAbs_ShapeEnum theShapeType,
    Selector_NameGenerator* theNameGenerator, const bool theGeometricalNaming = false);

  /// Returns the current sub-shape value (null if can not resolve)
  SELECTOR_EXPORT TopoDS_Shape value();

  /// Returns the naming name of the selection
  SELECTOR_EXPORT std::wstring name(Selector_NameGenerator* theNameGenerator);

  /// Makes the current local selection becomes all sub-shapes with same base geometry.
  SELECTOR_EXPORT void combineGeometrical(const TopoDS_Shape theContext);

  /// Stores the selected shape in he tree and returns true if shape found correctly
  SELECTOR_EXPORT bool solve(const TopoDS_Shape theContext);
};

#endif
