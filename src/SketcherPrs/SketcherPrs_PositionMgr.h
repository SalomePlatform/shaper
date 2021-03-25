// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef SketcherPrs_PositionMgr_H
#define SketcherPrs_PositionMgr_H

#include "SketcherPrs.h"
#include "SketcherPrs_SymbolPrs.h"

#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt.h>
#include <gp_Pnt.hxx>
#include <ModelAPI_Object.h>

#include <map>
#include <array>

/**
* \ingroup GUI
* A class Position Manager which manages position of constraints symbols along a source object line.
* it expects that symbol icons have size 16x16 px
*/
class SketcherPrs_PositionMgr
{
public:
  /// Returns current instance of position manager
  SKETCHERPRS_EXPORT static SketcherPrs_PositionMgr* get();

  /// Returns position of symbol for the given presentation
  /// \param theLine constrained object
  /// \param thePrs a presentation of constraint
  /// \param theStep step between symbols
  SKETCHERPRS_EXPORT gp_Pnt getPosition(ObjectPtr theLine, const SketcherPrs_SymbolPrs* thePrs,
                                        double theStep = 20, GeomPointPtr thePnt = GeomPointPtr());

  /// Deletes constraint object from internal structures. Has to be called on constraint delete.
  /// \param thePrs a constraint presentation
  SKETCHERPRS_EXPORT void deleteConstraint(const SketcherPrs_SymbolPrs* thePrs);

  /// Cleares all stored positions for all constraints
  void clearAll()  { myShapes.clear(); myPntShapes.clear(); }

private:
  /// Constructor
  SketcherPrs_PositionMgr();

  /// Returns position index of the given constraint
  /// \param theLine constrained object
  /// \param thePrs a presentation of constraint
  int getPositionIndex(ObjectPtr theLine, const SketcherPrs_SymbolPrs* thePrs);

  /// Returns position index of the given constraint around a point
  /// \param theLine constrained object
  /// \param thePrs a presentation of constraint
  const std::array<int, 2>& getPositionIndex(GeomPointPtr thePos,
                                      const SketcherPrs_SymbolPrs* thePrs);

  /// Returns position of a constraint around a point
  /// \param theLine a base object of the constraint
  /// \param thePrs a presentation of the constraint symbol
  /// \param theStep step from base point
  /// \param thePnt a base point
  gp_Pnt getPointPosition(ObjectPtr theLine, const SketcherPrs_SymbolPrs* thePrs,
                          double theStep, GeomPointPtr thePnt);

  static bool isPntConstraint(const std::string& theName);

private:
  typedef std::map<const SketcherPrs_SymbolPrs*, int> PositionsMap;
  typedef std::map<const ModelAPI_Feature*, std::array<int, 2>> FeaturesMap;

  /// The map contains position index
  std::map<ObjectPtr, PositionsMap> myShapes;

  /// The map contains position of index for constraints around a point
  FeaturesMap myPntShapes;
};

#endif