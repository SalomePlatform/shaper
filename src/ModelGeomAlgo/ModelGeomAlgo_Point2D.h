// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModelGeomAlgo_Point2D_H
#define ModelGeomAlgo_Point2D_H

#include "ModelGeomAlgo.h"
#include "ModelAPI_Feature.h"

class ModelAPI_Feature;
class ModelAPI_Object;
class GeomAPI_Shape;
class GeomAPI_Pnt;
class GeomAPI_Dir;

class GeomDataAPI_Point2D;

#include <memory>
#include <string>
#include <set>
#include <map>


class ModelGeomAlgo_Point2D
{
public:
  /// Searches Point2D attribute of reference of the attribute of given feature
  /// \param theFeature a feature to obtain AttributeRefAttr
  /// \param theAttribute a name of AttributeRefAttr on the given feature
  /// \param theObjectFeatureKind a feature kind in object of attribute that satisfies the search
  /// \param theObjectFeatureAttribute a feature attribute in object that satisfies the search
  /// \param isSkipFeatureAttributes a boolean value if coincidences to the feature attributes
  /// \returns found point attribute or NULL
  static MODELGEOMALGO_EXPORT std::shared_ptr<GeomDataAPI_Point2D> getPointOfRefAttr(
                        ModelAPI_Feature* theFeature,
                        const std::string& theAttribute,
                        const std::string& theObjectFeatureKind = "",
                        const std::string& theObjectFeatureAttribute = "");

  /// Fills container of point 2D attributes, which refer to the feature through the references
  /// features with the given kind
  /// \param theObject an object where references should be searched (e.g. a sketch line or result)
  /// \param theReferenceFeatureKind a kind of the feature to be processed
  ///                                (e.g. coincidence constraint)
  /// \param theAttributes a container of found point 2D attributes
  /// \param theObjectFeatureKind a feature kind in object of attribute that satisfies the search
  /// \param theObjectFeatureAttribute a feature attribute in object that satisfies the search
  /// \param isSkipFeatureAttributes a boolean value if coincidences to the feature attributes
  /// should be skipped
  /// \returns found point attribute or NULL
  static MODELGEOMALGO_EXPORT
        void getPointsOfReference(const std::shared_ptr<ModelAPI_Object>& theObject,
                                  const std::string& theReferenceFeatureKind,
                                  std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                                  const std::string& theObjectFeatureKind = "",
                                  const std::string& theObjectFeatureAttribute = "",
                                  const bool isSkipFeatureAttributes = true);

  /// Find points of intersection between the shape of the feature and all features in the sketch
  /// \param theBaseFeature a feature: line, arc or circle that will be intersected
  /// \param theFeatures a container of features to intersect with the base feature
  /// \param thePoints a container of 3D points belong to the shape
  /// \param theObjectToPoint a container of object to point
  typedef std::map<std::shared_ptr<GeomAPI_Pnt>,
                   std::pair<std::list<std::shared_ptr<GeomDataAPI_Point2D> >,
                             std::list<std::shared_ptr<ModelAPI_Object> > > > PointToRefsMap;

  static MODELGEOMALGO_EXPORT void getPointsIntersectedShape(
                  const std::shared_ptr<ModelAPI_Feature>& theBaseFeature,
                  const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                  PointToRefsMap& thePointToAttributeOrObject);

  static MODELGEOMALGO_EXPORT std::list<std::shared_ptr<GeomAPI_Pnt> > getSetOfPntIntersectedShape(
                  const std::shared_ptr<ModelAPI_Feature>& theBaseFeature,
                  const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures);

  /// Removes attributes which points are out of the base shape
  /// \param theBaseShape a shape of check
  /// \param theAttributes a container of point 2D attributes
  /// \param theOrigin origin of a plane to generate 3D point by 2D attribute point
  /// \param theDirX plane X direction to generate 3D point by 2D attribute point
  /// \param theDirY plane X direction to generate 3D point by 2D attribute point
  /// \param thePoints a container of 3D points belong to the shape
  /// \param theAttributeToPoint a container of attribute to point
  static MODELGEOMALGO_EXPORT void getPointsInsideShape(
                        const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                        const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                        const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                        const std::shared_ptr<GeomAPI_Dir>& theDirX,
                        const std::shared_ptr<GeomAPI_Dir>& theDirY,
                        PointToRefsMap& thePointToAttributeOrObject);

  /// Removes attributes which points are out of the base shape
  /// \param theBaseShape a shape of check
  /// \param theAttributes a container of point 2D attributes
  /// \param theOrigin origin of a plane to generate 3D point by 2D attribute point
  /// \param theDirX plane X direction to generate 3D point by 2D attribute point
  /// \param theDirY plane X direction to generate 3D point by 2D attribute point
  /// \param thePoints a container of 3D points belong to the shape
  /// \param theAttributeToPoint a container of attribute to point
  static MODELGEOMALGO_EXPORT void getPointsInsideShape_p(
                              const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                              const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                              const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                              const std::shared_ptr<GeomAPI_Dir>& theDirX,
                              const std::shared_ptr<GeomAPI_Dir>& theDirY,
                              std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                              std::map<std::shared_ptr<GeomDataAPI_Point2D>,
                                       std::shared_ptr<GeomAPI_Pnt> >& theAttributeToPoint);
  /// Finds projected point to the given shape line
  /// \param theBaseShape a shape of check
  /// \param thePoint [in] a point to project
  /// \param theProjectedPoint [out] a projected point
  static MODELGEOMALGO_EXPORT bool isPointOnEdge(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                          const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                                          std::shared_ptr<GeomAPI_Pnt>& theProjectedPoint);

  /// Finds projected point to the given shape, which does not placed on the shape boundaries
  /// \param theBaseShape a shape of check
  /// \param thePoint [in] a point to project
  /// \param theProjectedPoint [out] a projected point
  static MODELGEOMALGO_EXPORT bool isInnerPointOnEdge(
                                          const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                          const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                                          std::shared_ptr<GeomAPI_Pnt>& theProjectedPoint);

#ifdef _DEBUG
  /// Return feature name, kind and point values united in a string info
  /// \param theFeature an investigated feature
  /// \param theAttributesOnly a container of necessary attributes, if empty, all
  /// \return string value
  static MODELGEOMALGO_EXPORT std::string getPontAttributesInfo(
                      const std::shared_ptr<ModelAPI_Feature>& theFeature,
                      const std::set<std::shared_ptr<ModelAPI_Attribute> >& theAttributesOnly);

  /// Return point attribute string info
  /// \param theAttribute an investigated attribute
  /// \return string value
  static MODELGEOMALGO_EXPORT std::string getPointAttributeInfo(
                      const std::shared_ptr<ModelAPI_Attribute>& theAttribute);

#endif
};

#endif
