// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_PositionMgr.h
// Created:     13 March 2015
// Author:      Vitaly SMETANNIKOV

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
  gp_Pnt getPosition(ObjectPtr theLine, Handle(SketcherPrs_SymbolPrs) thePrs, double theStep = 20);

  /// Deletes constraint object from internal structures. Has to be called on constraint delete.
  /// \param thePrs a constraint presentation
  void deleteConstraint(Handle(SketcherPrs_SymbolPrs) thePrs);

private:
  /// Constructor
  SketcherPrs_PositionMgr();

  /// Returns position index of the given constraint
  /// \param theLine constrained object 
  /// \param thePrs a presentation of constraint
  int getPositionIndex(ObjectPtr theLine, Handle(SketcherPrs_SymbolPrs) thePrs);

private:
  typedef std::map<void*, int> PositionsMap;

  /// The map which contains position of presentation
  PositionsMap myIndexes;

  /// The map contains position index 
  std::map<ObjectPtr, PositionsMap> myShapes;
};

#endif