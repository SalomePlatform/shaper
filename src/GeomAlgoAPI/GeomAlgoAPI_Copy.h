// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Copy.h
// Created:     06 Sept 2016
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Copy_H_
#define GeomAlgoAPI_Copy_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Copy
/// \ingroup DataAlgo
/// \brief Duplication of a shape.
class GeomAlgoAPI_Copy : public GeomAlgoAPI_MakeShape
{
public:

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Copy(const std::shared_ptr<GeomAPI_Shape> theShape,
                                      const bool theCopyGeom = true,
                                      const bool theCopyMesh = false);

private:
  /// Builds resulting shape.
  void build(const std::shared_ptr<GeomAPI_Shape> theShape,
             const bool theCopyGeom = true,
             const bool theCopyMesh = false);
};

#endif
