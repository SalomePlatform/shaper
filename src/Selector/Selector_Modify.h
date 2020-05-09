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

#ifndef Selector_Modify_H_
#define Selector_Modify_H_

#include "Selector_Algo.h"

#include <TNaming_NamedShape.hxx>

#include <list>

/**\class Selector_Modify
 * \ingroup DataModel
 * \brief Kind of selection algorithm: sub-shape is modification of primitive at some
 * final stage.
 */
class Selector_Modify: public Selector_Algo
{
  TDF_Label myFinal; ///< final label of the primitive or generation, where the value is
  TDF_LabelList myBases; ///< initial labels that contain shapes that produce the modification
  int myWeakIndex; ///< weak index in case modification produces several shapes
  bool myRecomputeWeakIndex; ///< if the index is stored with old (unstable) order, recompute it
public:
  /// Initializes the selection of this kind by list of named shapes where the modification results
  /// are presented and the selected value.
  SELECTOR_EXPORT bool select(NCollection_List<Handle(TNaming_NamedShape)>& theModifList,
    const TopoDS_Shape theContext, const TopoDS_Shape theValue);

  /// Stores the name to the label and sub-labels tree
  SELECTOR_EXPORT virtual void store() override;

  /// Restores the selected shape by the topological naming kept in the data structure
  /// Returns true if it can restore structure correctly
  SELECTOR_EXPORT virtual bool restore() override;

  /// Restores the selected shape by the topological name string.
  /// Returns not empty label of the context.
  SELECTOR_EXPORT virtual TDF_Label restoreByName(std::string theName,
    const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator) override;

  /// Updates the current shape by the stored topological name
  SELECTOR_EXPORT virtual bool solve(const TopoDS_Shape& theContext) override;

  /// Returns the naming name of the selection
  SELECTOR_EXPORT virtual std::string name(Selector_NameGenerator* theNameGenerator) override;

private:
  /// Initializes selector
  Selector_Modify();
  /// Searches the final shapes presented in all results from bases basing on modification fields
  void findModificationResult(TopoDS_ListOfShape& theCommon);

  friend class Selector_Algo;
};

#endif
