// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Sewing.h
// Created:     25 April 2016
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Sewing_H_
#define GeomAlgoAPI_Sewing_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShape.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Sewing
/// \ingroup DataAlgo
/// \brief Allows creation of connected topology (shells) from a set of separate topological elements (faces).
class GeomAlgoAPI_Sewing : public GeomAlgoAPI_MakeShape
{
public:
  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sewing(const ListOfShape& theShapes);

  /// \return the list of shapes modified from the shape \a theShape.
  /// \param[in] theShape base shape.
  /// \param[out] theHistory modified shapes.
  GEOMALGOAPI_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theShapes);
};

#endif
