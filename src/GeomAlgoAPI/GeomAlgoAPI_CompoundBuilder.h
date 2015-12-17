// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_CompoundBuilder.h
// Created:     24 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef GeomAlgoAPI_CompoundBuilder_H_
#define GeomAlgoAPI_CompoundBuilder_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <memory>

#include <list>

/**\class GeomAlgoAPI_CompoundBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_CompoundBuilder
{
 public:
  /// Creates compund of the given shapes
  /// \param theShapes a list of shapes
  static std::shared_ptr<GeomAPI_Shape> compound(
      std::list<std::shared_ptr<GeomAPI_Shape> > theShapes);

  /// Produces the integerr identifier of the shape theSub in theContext (needed for 
  /// groups export to old GEOM)
  /// \returns zero if theSub not found in theContext
  static int id(
      std::shared_ptr<GeomAPI_Shape> theContext, std::shared_ptr<GeomAPI_Shape> theSub);
};

#endif
