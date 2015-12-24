// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Transform.h
// Created:     29 July 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Transform_H_
#define GeomAlgoAPI_Transform_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Trsf.h>

/// \class GeomAlgoAPI_Transform
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by transformating it.
class GeomAlgoAPI_Transform : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Creates an object which is obtained from current object by transformating it.
  /// \param[in] theSourceShape  a shape to be transformed.
  /// \param[in] theTrsf         transformation.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Transform(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Trsf>  theTrsf);

  /// \return the transformation.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Trsf> transformation() const;

private:
  /// Builds resulting shape.
  void build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
             std::shared_ptr<GeomAPI_Trsf>  theTrsf);

private:
  std::shared_ptr<GeomAPI_Trsf> myTrsf;
};

#endif
