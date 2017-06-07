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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SketcherPrs_PositionMgr_H
#define SketcherPrs_PositionMgr_H

#include "SketcherPrs_SymbolPrs.h"

#include <GeomAPI_Shape.h>
#include <gp_Pnt.hxx>
#include <ModelAPI_Object.h>

#include <map>

/**
* \ingroup GUI
* A class Position Manager which manages position of constraints symbols along a source object line.
* it expects that symbol icons have size 16x16 px
*/
class SketcherPrs_PositionMgr
{
public:
  /// Returns current instance of position manager
  static SketcherPrs_PositionMgr* get();

  /// Returns position of symbol for the given presentation
  /// \param theLine constrained object
  /// \param thePrs a presentation of constraint
  /// \param theStep step between symbols
  gp_Pnt getPosition(ObjectPtr theLine, const SketcherPrs_SymbolPrs* thePrs, double theStep = 20);

  /// Deletes constraint object from internal structures. Has to be called on constraint delete.
  /// \param thePrs a constraint presentation
  void deleteConstraint(const SketcherPrs_SymbolPrs* thePrs);

private:
  /// Constructor
  SketcherPrs_PositionMgr();

  /// Returns position index of the given constraint
  /// \param theLine constrained object
  /// \param thePrs a presentation of constraint
  int getPositionIndex(ObjectPtr theLine, const SketcherPrs_SymbolPrs* thePrs);

private:
  typedef std::map<const SketcherPrs_SymbolPrs*, int> PositionsMap;

  /// The map which contains position of presentation
  PositionsMap myIndexes;

  /// The map contains position index
  std::map<ObjectPtr, PositionsMap> myShapes;
};

#endif