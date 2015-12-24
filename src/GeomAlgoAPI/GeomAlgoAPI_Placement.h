// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Placement.h
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAlgoAPI_Placement_H_
#define GeomAlgoAPI_Placement_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Trsf.h>

/// \class GeomAlgoAPI_Placement
/// \ingroup DataAlgo
/// \brief Creates the copied object which sub-element is placed on the given element
class GeomAlgoAPI_Placement : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Creates an object which is obtained from current object by transformation calculated
  ///        as a movement of the source object to be coincident with the destination object
  /// \param[in] theSourceSolid  a shape to be moved
  /// \param[in] theDestSolid    invariant shape
  /// \param[in] theSourceShape  a shape on the solid to be made coincident with destination object
  /// \param[in] theDestShape    destination object
  /// \param[in] theIsReverse    indicates that the solid materials should be on the same side against the destination plane
  /// \param[in] theIsCentering  indicates the planes should be centered
  /// \param[in] theSimpleTransform makes just transformation of shape without changing of topology or geometry
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Placement(const std::shared_ptr<GeomAPI_Shape> theSourceSolid,
                                           const std::shared_ptr<GeomAPI_Shape> theDestSolid,
                                           const std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           const std::shared_ptr<GeomAPI_Shape> theDestShape,
                                           const bool theIsReverse = false,
                                           const bool theIsCentering = false,
                                           const bool theSimpleTransform = false);

  /// Returns the simple transformation
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Trsf> transformation() const;

private:
  /// builds resulting shape
  void build(const std::shared_ptr<GeomAPI_Shape>& theSourceSolid,
             const std::shared_ptr<GeomAPI_Shape>& theDestSolid,
             const std::shared_ptr<GeomAPI_Shape>& theSourceShape,
             const std::shared_ptr<GeomAPI_Shape>& theDestShape,
             const bool theIsReverse,
             const bool theIsCentering,
             const bool theSimpleTransform);

private:
  std::shared_ptr<GeomAPI_Trsf> myTrsf; ///< transformation of the shape in case theSimpleTransform
};

#endif
