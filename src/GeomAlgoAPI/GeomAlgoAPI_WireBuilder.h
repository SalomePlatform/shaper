// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_WireBuilder.h
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_WireBuilder_H_
#define GeomAlgoAPI_WireBuilder_H_

#include "GeomAlgoAPI.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_WireBuilder
/// \ingroup DataAlgo
/// \brief Allows to create wire-shapes by different parameters.
class GeomAlgoAPI_WireBuilder
{
 public:
   /// \brief Creates a wire from edges and wires.
   /// \param[in] theShapes list of shapes. Only edges and wires allowed.
   /// The edges are not to be consecutive. But they are to be all connected geometrically or topologically.
   /// \return wire created from theShapes. Empty in case of error or bad input.
   GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> wire(const ListOfShape& theShapes);
};

#endif
