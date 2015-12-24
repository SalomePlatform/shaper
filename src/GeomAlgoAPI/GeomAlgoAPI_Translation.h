// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Translation.h
// Created:     8 June 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Translation_H_
#define GeomAlgoAPI_Translation_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Translation
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by moving it along the axis.
class GeomAlgoAPI_Translation : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Creates an object which is obtained from current object by moving it along the axis.
  /// \param[in] theSourceShape  a shape to be moved.
  /// \param[in] theAxis         movement axis.
  /// \param[in] theDistance     movement distance.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                             std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                             double                         theDistance);

private:
  /// Builds resulting shape.
  void build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
             std::shared_ptr<GeomAPI_Ax1>   theAxis,
             double                         theDistance);
};

#endif
