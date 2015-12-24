// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_PaveFiller.h
// Created:     27 August 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_PaveFiller_H_
#define GeomAlgoAPI_PaveFiller_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_PaveFiller
/// \ingroup DataAlgo
/// \brief Finds the common parts from the list of shapes and breaks it to shapes with shared subshapes.
class GeomAlgoAPI_PaveFiller : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Constructor.
  /// \param[in] theListOfShape list of shape which should be splitted.
  /// \param[in] theIsMakeCompSolids if true gather shapes with shared faces to compsolids.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_PaveFiller(const ListOfShape& theListOfShape, const bool theIsMakeCompSolids = false);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theListOfShape, const bool theIsMakeCompSolids);
};

#endif
