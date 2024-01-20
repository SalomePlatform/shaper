// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef Selector_FilterByNeighbors_H_
#define Selector_FilterByNeighbors_H_

#include "Selector_AlgoWithSubs.h"

#include <list>

/**\class Selector_FilterByNeighbors
 * \ingroup DataModel
 * \brief Kind of selection algorithm: selected shape is identified by neighbor faces of
 * the same type. Or neighbors of neighbors are considered, etc.
 * be named one by one.
 */
class Selector_FilterByNeighbors: public Selector_AlgoWithSubs
{
  TopAbs_ShapeEnum myShapeType; ///< type of this shape
  std::list<int> myNBLevel; ///< list of integers corresponding to subs neighborhood level
  TDF_Label myContext; ///< label where the context of filter is stored (for naming name strings)
public:
  /// Initializes the selection of this kind
  SELECTOR_EXPORT bool select(
    const TDF_Label theContextLab, const TopoDS_Shape theContext, const TopoDS_Shape theValue);

  /// Stores the name to the label and sub-labels tree
  SELECTOR_EXPORT virtual void store() override;

  /// Restores the selected shape by the topological naming kept in the data structure
  /// Returns true if it can restore structure correctly
  SELECTOR_EXPORT virtual bool restore() override;

  /// Restores the selected shape by the topological name string.
  /// Returns not empty label of the context.
  SELECTOR_EXPORT virtual TDF_Label restoreByName(std::wstring theName,
    const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator) override;

  /// Updates the current shape by the stored topological name
  SELECTOR_EXPORT virtual bool solve(const TopoDS_Shape& theContext) override;

  /// Returns the naming name of the selection
  SELECTOR_EXPORT virtual std::wstring name(Selector_NameGenerator* theNameGenerator) override;
private:
  /// Initializes selector
  Selector_FilterByNeighbors();

  friend class Selector_Algo;
};

#endif
