// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelGeomAlgo_Point2D.h
// Created:     20 Jul 2016
// Author:      Natalia ERMOLAEVA

#ifndef ModelGeomAlgo_Point2D_H
#define ModelGeomAlgo_Point2D_H

#include "ModelGeomAlgo.h"

class ModelAPI_Feature;
class ModelAPI_Object;
class GeomAPI_Shape;
class GeomAPI_Pnt;
class GeomAPI_Dir;

class GeomDataAPI_Point2D;

#include <memory>
#include <string>
#include <set>


namespace ModelGeomAlgo_Point2D {

  /// Searches Point2D attribute of reference of the attribute of given feature
  /// \param theFeature a feature to obtain AttributeRefAttr
  /// \param theAttribute a name of AttributeRefAttr on the given feature
  /// \param theObjectFeatureKind a feature kind in object of attribute that satisfies the search
  /// \param theObjectFeatureAttribute a feature attribute in object that satisfies the search
  /// \returns found point attribute or NULL
  MODELGEOMALGO_EXPORT std::shared_ptr<GeomDataAPI_Point2D> getPointOfRefAttr(
                                                ModelAPI_Feature* theFeature,
                                                const std::string& theAttribute,
                                                const std::string& theObjectFeatureKind = "",
                                                const std::string& theObjectFeatureAttribute = "");

  /// Fills container of point 2D attributes, which refer to the feature through the references
  /// features with the given kind
  /// \param theObject an object where references should be searched (e.g. a sketch line or result)
  /// \param theReferenceFeatureKind a kind of the feature to be processed (e.g. coincidence constraint)
  /// \param theAttributes a container of found point 2D attributes
  /// \param theObjectFeatureKind a feature kind in object of attribute that satisfies the search
  /// \param theObjectFeatureAttribute a feature attribute in object that satisfies the search
  /// \returns found point attribute or NULL
  MODELGEOMALGO_EXPORT void getPointsOfReference(const std::shared_ptr<ModelAPI_Object>& theObject,
                                          const std::string& theReferenceFeatureKind,
                                          std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                                          const std::string& theObjectFeatureKind = "",
                                          const std::string& theObjectFeatureAttribute = "");

  /// Removes attributes which points are out of the base shape
  /// \param theBaseShape a shape of check
  /// \param theAttributes a container of point 2D attributes
  /// \param theOrigin origin of a plane to generate 3D point by 2D attribute point
  /// \param theDirX plane X direction to generate 3D point by 2D attribute point
  /// \param theDirY plane X direction to generate 3D point by 2D attribute point
  /// \param thePoints a container of 3D points belong to the shape
  MODELGEOMALGO_EXPORT void getPointsInsideShape(
                              const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                              const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                              const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                              const std::shared_ptr<GeomAPI_Dir>& theDirX,
                              const std::shared_ptr<GeomAPI_Dir>& theDirY,
                              std::set<std::shared_ptr<GeomAPI_Pnt> >& thePoints);

  /// Finds projected point to the given shape line
  /// \param theBaseShape a shape of check
  /// \param thePoint [in] a point to project
  /// \param theProjectedPoint [out] a projected point
  MODELGEOMALGO_EXPORT bool isPointOnEdge(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                          const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                                          std::shared_ptr<GeomAPI_Pnt>& theProjectedPoint);
}

#endif
