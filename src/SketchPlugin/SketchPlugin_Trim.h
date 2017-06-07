// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SketchPlugin_Trim_H_
#define SketchPlugin_Trim_H_

#include "SketchPlugin.h"

#include "GeomAPI_IPresentable.h"
#include <ModelAPI_IReentrant.h>
#include <SketchPlugin_Sketch.h>

class GeomDataAPI_Point2D;
class ModelAPI_Feature;
class ModelAPI_Result;
class ModelAPI_Object;

typedef std::pair<std::string, std::shared_ptr<GeomDataAPI_Point2D> > IdToPointPair;

/** \class SketchPlugin_Trim
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint trimming object. Entities for split:
 */
class SketchPlugin_Trim : public SketchPlugin_Feature, public GeomAPI_IPresentable,
                          public ModelAPI_IReentrant
{
 public:
  /// Split constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_TRIM_ID("SketchTrim");
    return MY_TRIM_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Trim::ID();
    return MY_KIND;
  }

  /// The value parameter for the constraint
  inline static const std::string& SELECTED_OBJECT()
  {
    static const std::string MY_SELECTED_OBJECT("SelectedObject");
    return MY_SELECTED_OBJECT;
  }

  /// Start 2D point of the split segment
  inline static const std::string& SELECTED_POINT()
  {
    static const std::string MY_SELECTED_POINT("SelectedPoint");
    return MY_SELECTED_POINT;
  }

  /// The value parameter for the preview object
  inline static const std::string& PREVIEW_OBJECT()
  {
    static const std::string MY_PREVIEW_OBJECT("PreviewObject");
    return MY_PREVIEW_OBJECT;
  }

  /// Start 2D point of the split segment
  inline static const std::string& PREVIEW_POINT()
  {
    static const std::string MY_PREVIEW_POINT("PreviewPoint");
    return MY_PREVIEW_POINT;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Reimplemented from ModelAPI_Feature::isMacro()
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  /// This is necessary to perform execute only by apply the feature
  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// \brief Use plugin manager for features creation
  SketchPlugin_Trim();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Moves the feature : Empty
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY) {};

  /// Apply information of the message to current object. It fills selected point and object
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage);

  typedef std::map<std::shared_ptr<GeomAPI_Pnt>,
                   std::pair<std::list<std::shared_ptr<GeomDataAPI_Point2D> >,
                             std::list<std::shared_ptr<ModelAPI_Object> > > > PointToRefsMap;

  static void fillObjectShapes(const std::shared_ptr<ModelAPI_Object>& theObject,
    const std::shared_ptr<ModelAPI_Object>& theSketch,
    std::map<std::shared_ptr<ModelAPI_Object>, std::set<GeomShapePtr> >& theCashedShapes,
    std::map<std::shared_ptr<ModelAPI_Object>, PointToRefsMap>& theObjectToPoints);

private:
  bool setCoincidenceToAttribute(const AttributePtr& theAttribute,
            const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences);
  /// Move tangency constraint to the feature if it is geometrically closely to it
  /// \param theAttribute an attribute of a tangent constraint feature
  /// \param theFeature a feature that can be set into the attribute
  bool moveTangency(const AttributePtr& theAttribute, const FeaturePtr& theFeature);

  GeomShapePtr getSubShape(const std::string& theObjectAttributeId,
                           const std::string& thePointAttributeId);

  /// Returns geom point attribute of the feature bounds. It processes line or arc.
  /// For circle feature, the result attributes are null
  /// \param theFeature a source feature
  /// \param theStartPointAttr an out attribute to start point
  /// \param theStartPointAttr an out attribute to end point
  void getFeaturePoints(const FeaturePtr& theFeature,
                        std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                        std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr);

  /// Obtains those constraints of the feature that should be modified. output maps contain
  /// point of coincidence and attribute id to be modified after split
  /// \param theFeaturesToDelete [out] constrains that will be deleted after split
  /// \param theFeaturesToUpdate [out] constrains that will be updated after split
  void getConstraints(std::set<std::shared_ptr<ModelAPI_Feature>>& theFeaturesToDelete,
                      std::set<FeaturePtr>& theFeaturesToUpdate);

  /// Obtains references to feature point attributes and to feature,
  /// e.g. for feature line: 1st container is
  ///             <1st line point, list<entity_a in distance, entity_b in parallel> >
  ///             <2nd line point, list<> >
  ///      for feature circle 2nd container is <entity_a in Radius, entity_b in equal, ...>
  /// \param theFeature an investigated feature
  /// \param theRefs a container of list of referenced attributes
  void getRefAttributes(const FeaturePtr& theFeature,
                        std::map<AttributePtr, std::list<AttributePtr> >& theRefs,
                        std::list<AttributePtr>& theRefsToFeature);

  /// Move constraints from attribute of base feature to attribute after modification
  /// \param theBaseRefAttributes container of references to the attributes of base feature
  /// \param theModifiedAttributes container of attributes placed instead of base attributes
  /// at the same place
  void updateRefAttConstraints(
               const std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
               const std::set<std::pair<AttributePtr, AttributePtr> >& theModifiedAttributes,
               std::set<std::shared_ptr<ModelAPI_Feature>>& theFeaturesToDelete);

  /// Remove references constraints from attribute of base feature refer to the given attribute
  /// \param theAttribute an attribute
  /// \param theModifiedAttributes modifiable container of attributes
  void removeReferencesToAttribute(const AttributePtr& theAttribute,
                  std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes);

   /// Updates line length if it exist in the list
  /// \param theFeaturesToUpdate a constraints container
  void updateFeaturesAfterTrim(const std::set<FeaturePtr>& theFeaturesToUpdate);

  /// Make the base object is splitted by the point attributes
  /// \param theBaseRefAttributes container of references to the attributes of base feature
  /// \param thePoints a list of points where coincidences will be build
  /// \param theModifiedAttributes a container of attribute on base
  /// feature to attribute on new feature
  /// \return new line if it was created
  FeaturePtr trimLine(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes);

  /// Make the base object is splitted by the point attributes
  /// \param thePoints a list of points where coincidences will be build
  /// \return new line if it was created
  FeaturePtr trimArc(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
               const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
               std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
               std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
               std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes);

  /// Make the base object is splitted by the point attributes
  /// \param thePoints a list of points where coincidences will be build
  FeaturePtr trimCircle(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                  const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                  std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                  std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes);

  /// Correct the first and the second point to provide condition that the first is closer to
  /// the start point and the second point - to the last end of current segment. To rearrange
  /// them if this condition is not satisfied.
  /// \param theStartPointAttr a start point of a segment
  /// \param theEndPointAttr an end point of a segment
  /// \param theFirstPoint a start point of a segment
  /// \param theSecondPoint an end point of a segment
  void arrangePointsOnLine(const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                           const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr,
                           std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                           std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint) const;

  /// Correct the first and the second point to provide condition that the first is closer to
  /// the start point and the second point - to the last end of current segment. To rearrange
  /// them if this condition is not satisfied.
  /// \param theArc an arc to be split
  /// \param theStartPointAttr a start point of a segment
  /// \param theEndPointAttr an end point of a segment
  /// \param theFirstPoint a start point of a segment
  /// \param theSecondPoint an end point of a segment
  void arrangePointsOnArc(const FeaturePtr& theArc,
                          const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                          const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr,
                          std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                          std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint) const;

  /// Fill attribute by value of another attribute. It processes only Point 2D attributes.
  /// \param theModifiedAttribute an attribute of GeomDataAPI_Point2D on feature to be modified
  /// \param theSourceAttribute an attribute of GeomDataAPI_Point2D to obtain data
  void fillAttribute(const AttributePtr& theModifiedAttribute,
                     const AttributePtr& theSourceAttribute);

  /// Fill attribute by value of another attribute. It processes only Point 2D attributes.
  /// \param theModifiedAttribute an attribute of GeomDataAPI_Point2D on feature to be modified
  /// \param thePoint a point value
  void fillPointAttribute(const AttributePtr& theModifiedAttribute,
                          const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Creates a line feature filled by center of base feature and given points
  /// \param theBaseFeature another arc feature
  /// \param theFirstAttribute an attribute with coordinates for the start point
  /// \param theSecondAttribute an attribute with coordinates for the end point
  FeaturePtr createLineFeature(const FeaturePtr& theBaseFeature,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint);

  /// Creates an arc feature filled by center of base feature and given points
  /// \param theBaseFeature another arc feature
  /// \param theFirstAttribute an attribute with coordinates for the start point
  /// \param theSecondAttribute an attribute with coordinates for the end point
  FeaturePtr createArcFeature(const FeaturePtr& theBaseFeature,
                              const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                              const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint);

  /// Add feature coincidence constraint between given attributes
  /// \param theConstraintId a constraint index
  /// \param theFirstAttribute an attribute of further coincidence
  /// \param theSecondAttribute an attribute of further coincidence
  std::shared_ptr<ModelAPI_Feature> createConstraint(const std::string& theConstraintId,
                        const std::shared_ptr<ModelAPI_Attribute>& theFirstAttribute,
                        const std::shared_ptr<ModelAPI_Attribute>& theSecondAttribute);

  /// Add feature coincidence constraint between given attributes
  /// \param theConstraintId a constraint index
  /// \param theFirstAttribute an attribute of further coincidence
  /// \param theSecondObject an object of further coincidence
  std::shared_ptr<ModelAPI_Feature> createConstraintToObject(const std::string& theConstraintId,
                        const std::shared_ptr<ModelAPI_Attribute>& theFirstAttribute,
                        const std::shared_ptr<ModelAPI_Object>& theSecondObject);

  /// Add feature coincidence constraint between given attributes
  /// \param theConstraintId a constraint index
  /// \param theFirstAttribute an attribute of further coincidence
  /// \param theFirstAttribute an attribute of further coincidence
  std::shared_ptr<ModelAPI_Feature> createConstraintForObjects(const std::string& theConstraintId,
                        const std::shared_ptr<ModelAPI_Object>& theFirstObject,
                        const std::shared_ptr<ModelAPI_Object>& theSecondObject);

  /// Result result of the feature to build constraint with. For arc, circle it is an edge result.
  /// \param theFeature a feature
  /// \return result object
  std::shared_ptr<ModelAPI_Result> getFeatureResult(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature);

private:
  void findShapePoints(const std::string& theObjectAttributeId,
                       const std::string& thePointAttributeId,
                       std::shared_ptr<GeomAPI_Pnt>& aStartPoint,
                       std::shared_ptr<GeomAPI_Pnt>& aLastPoint);

  std::shared_ptr<GeomAPI_Pnt2d> convertPoint(const std::shared_ptr<GeomAPI_Pnt>& thePoint);

private:
  std::map<std::shared_ptr<ModelAPI_Object>, std::set<GeomShapePtr> > myCashedShapes;
  std::map<std::shared_ptr<ModelAPI_Object>, PointToRefsMap> myObjectToPoints;
};

#endif
