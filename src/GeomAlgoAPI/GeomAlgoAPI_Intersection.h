// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Intersection.h
// Created:     16 Feb 2016
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Intersection_H_
#define GeomAlgoAPI_Intersection_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Intersection
/// \ingroup DataAlgo
/// \brief Performs the intersection operations.
class GeomAlgoAPI_Intersection : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Constructor.
  /// \param[in] theObjects list of objects.
  /// \param[in] theTools list of tools.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Intersection(const ListOfShape& theObjects,
                                              const ListOfShape& theTools);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theObjects,
             const ListOfShape& theTools);
};

#endif
