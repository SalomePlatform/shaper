// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Split.h
// Created: 04 May 2017
// Author:  Natalia ERMOLAEVA

#ifndef SketchPlugin_Split_H_
#define SketchPlugin_Split_H_

#include "SketchPlugin.h"

#include "GeomAPI_IPresentable.h"
#include <ModelAPI_IReentrant.h>

#include <SketchPlugin_Sketch.h>

class GeomDataAPI_Point2D;
class ModelAPI_Feature;
class ModelAPI_Result;

typedef std::pair<std::string, std::shared_ptr<GeomDataAPI_Point2D> > IdToPointPair;

/** \class SketchPlugin_Split
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint splitting object. Entities for split:
 * - Linear segment by point(s) on this line
 * - Arc by point(s) on this arc
 * - Circle by at least 2 split-points on this circle
 *
 * The following constraints will be applied after split to keep the divided segments geometry:
 * - Coincident constraints for both parts of created segments in the point of splitting
 * - For linear segments parallel, for circles - tangent constraint, for arc - tangent and equal
 *   constraints. In case of three segments in result two couple of constraints are created
 *    - parallel and equal constraints: the first is between 1st and middle entity, the second is
 *      between 1st and 3rd.
 *    - tangency constraints: the first between 1st and 2nd, the second between 2nd and 3rd.
 * - Constraints assigned to the feature before split operation are assigned after using rules:
 *    - Coincident constraints are assigned to the segment where they belong to. Segment of split
 *      has only a coincidence to the neighbor segment. All constraints used in the splitting of
 *      this segment are moved to point of neighbor segment. If constraint was initially built
 *      to the point of splitting segment, it stays for this point.
 *    - Geometrical and dimensional constraints are assigned to one of result segment, not the
 *      selected for split. In case of 3 result segments, this will be the segment nearest to the
 *      start point of arc/line.
 *    - Replication constraint used split feature will be deleted in the same way as it is deleted
 *      by any of entity delete in sketch which is used in this constraint.
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::VALUE() contains reference object to be splitted
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B() for the points of split;
 *
 */
class SketchPlugin_Split : public SketchPlugin_Feature, public GeomAPI_IPresentable,
                           public ModelAPI_IReentrant
{
 public:
  /// Split constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_SPLIT_ID("SketchSplit");
    return MY_SPLIT_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Split::ID();
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

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  /// This is necessary to perform execute only by apply the feature
  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// \brief Use plugin manager for features creation
  SketchPlugin_Split();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Moves the feature : Empty
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY) {};

  /// Apply information of the message to current object. It fills selected point and object
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage);

private:
  /// Fulfill an internal container by shapes obtained from the parameter object
  /// Shapes are result of split operation by points coincident to shape of the object
  /// \param theObject a source object (will be splitted)
  /// \param theSketch a sketch object
  void fillObjectShapes(const ObjectPtr& theObject, const ObjectPtr& theSketch);

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

  /// Returns cast of attribute to geometrical point if the attribute is a ref attr attribute
  /// \param theAttribute an attribute
  /// \param geom point 2D or NULL
  std::shared_ptr<GeomDataAPI_Point2D> getPointOfRefAttr(const AttributePtr& theAttribute);

  /// Obtains those constraints of the feature that should be modified. output maps contain
  /// point of coincidence and attribute id to be modified after split
  /// \param theFeaturesToDelete [out] constrains that will be deleted after split
  /// \param theFeaturesToUpdate [out] constrains that will be updated after split
  /// \param theTangentFeatures  [out] tangent feature to be connected to new feature
  /// \param theCoincidenceToFeature [out] coincidence to feature to be connected to new feature
  /// \param theCoincidenceToPoint [out] coincidence to point be connected to new feature
  void getConstraints(std::set<std::shared_ptr<ModelAPI_Feature>>& theFeaturesToDelete,
              std::set<std::shared_ptr<ModelAPI_Feature>>& theFeaturesToUpdate,
              std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToFeature);

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

  /// Move coincidence constraint from feature to point if it is found
  /// \param theCoincidenceToFeature coincidence to feature to be connected to new feature
  /// \param theFurtherCoincidences a list of points where coincidences will be build
  /// \param theFeatureResults created results after split where constaint might be connected
  /// \param theSplitFeature feature created by split, new coincidences to points should be created
  /// if theCoincidenceToFeature contains equal points
  void updateCoincidenceConstraintsToFeature(
      const std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToFeature,
      const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences,
      const std::set<ResultPtr>& theFeatureResults,
      const FeaturePtr& theSplitFeature);

  /// Move constraints from base feature to given feature
  /// \param theFeature a base feature
  /// \param theRefsToFeature list of attributes referenced to base feature
  void updateRefFeatureConstraints(const std::shared_ptr<ModelAPI_Result>& theFeatureBaseResult,
                                   const std::list<AttributePtr>& theRefsToFeature);

  /// Move constraints from attribute of base feature to attribute after modification
  /// \param theBaseRefAttributes container of references to the attributes of base feature
  /// \param theModifiedAttributes container of attributes placed instead of base attributes
  /// at the same place
  void updateRefAttConstraints(
               const std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
               const std::set<std::pair<AttributePtr, AttributePtr> >& theModifiedAttributes);

  /// Make the base object is splitted by the point attributes
  /// \param theSplitFeature a result split feature
  /// \param theBeforeFeature a feature between start point and the 1st point of split feature
  /// \param theAfterFeature a feature between last point of split feature and the end point
  /// \param thePoints a list of points where coincidences will be build
  /// \param theCreatedFeatures a container of created features
  /// \param theModifiedAttributes a container of attribute on base
  /// feature to attribute on new feature
  /// \return new line if it was created
  FeaturePtr splitLine(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
                 std::shared_ptr<ModelAPI_Feature>& theBeforeFeature,
                 std::shared_ptr<ModelAPI_Feature>& theAfterFeature,
                 std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                 std::set<std::shared_ptr<ModelAPI_Feature>>& theCreatedFeatures,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes);

  /// Make the base object is splitted by the point attributes
  /// \param theSplitFeature a result split feature
  /// \param theBeforeFeature a feature between start point and the 1st point of split feature
  /// \param theAfterFeature a feature between last point of split feature and the end point
  /// \param thePoints a list of points where coincidences will be build
  /// \param theCreatedFeatures a container of created features
  /// \return new arc if it was created
  FeaturePtr splitArc(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
                std::shared_ptr<ModelAPI_Feature>& theBeforeFeature,
                std::shared_ptr<ModelAPI_Feature>& theAfterFeature,
                std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                std::set<std::shared_ptr<ModelAPI_Feature>>& theCreatedFeatures,
                std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes);

  /// Make the base object is splitted by the point attributes
  /// \param theSplitFeature a result split feature
  /// \param theBeforeFeature a feature between start point and the 1st point of split feature
  /// \param theAfterFeature a feature between last point of split feature and the end point
  /// \param thePoints a list of points where coincidences will be build
  /// \param theCreatedFeatures a container of created features
  /// \return new arc if it was created
  FeaturePtr splitCircle(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
                   std::shared_ptr<ModelAPI_Feature>& theBeforeFeature,
                   std::shared_ptr<ModelAPI_Feature>& theAfterFeature,
                   std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                   std::set<std::shared_ptr<ModelAPI_Feature>>& theCreatedFeatures,
                   std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes);

  /// Correct the first and the second point to provide condition that the first is closer to
  /// the start point and the second point - to the last end of current segment. To rearrange
  /// them if this condition is not satisfied.
  /// \param theStartPointAttr a start point of a segment
  /// \param theEndPointAttr an end point of a segment
  /// \param theFirstPointAttr a start point of a segment
  /// \param theSecondPointAttr an end point of a segment
  void arrangePointsOnLine(const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                           const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr,
                           std::shared_ptr<GeomDataAPI_Point2D>& theFirstPointAttr,
                           std::shared_ptr<GeomDataAPI_Point2D>& theSecondPointAttr) const;

  /// Correct the first and the second point to provide condition that the first is closer to
  /// the start point and the second point - to the last end of current segment. To rearrange
  /// them if this condition is not satisfied.
  /// \param theArc an arc to be split
  /// \param theStartPointAttr a start point of a segment
  /// \param theEndPointAttr an end point of a segment
  /// \param theFirstPointAttr a start point of a segment
  /// \param theSecondPointAttr an end point of a segment
  void arrangePointsOnArc(const FeaturePtr& theArc,
                          const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                          const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr,
                          std::shared_ptr<GeomDataAPI_Point2D>& theFirstPointAttr,
                          std::shared_ptr<GeomDataAPI_Point2D>& theSecondPointAttr) const;

  /// Fill attribute by value of another attribute. It processes only Point 2D attributes.
  /// \param theModifiedAttribute an attribute of GeomDataAPI_Point2D on feature to be modified
  /// \param theSourceAttribute an attribute of GeomDataAPI_Point2D to obtain data
  void fillAttribute(const AttributePtr& theModifiedAttribute,
                     const AttributePtr& theSourceAttribute);

  /// Creates a line feature filled by center of base feature and given points
  /// \param theBaseFeature another arc feature
  /// \param theFirstAttribute an attribute with coordinates for the start point
  /// \param theSecondAttribute an attribute with coordinates for the end point
  FeaturePtr createLineFeature(const FeaturePtr& theBaseFeature,
                               const AttributePtr& theFirstPointAttr,
                               const AttributePtr& theSecondPointAttr);

  /// Creates an arc feature filled by center of base feature and given points
  /// \param theBaseFeature another arc feature
  /// \param theFirstAttribute an attribute with coordinates for the start point
  /// \param theSecondAttribute an attribute with coordinates for the end point
  FeaturePtr createArcFeature(const FeaturePtr& theBaseFeature,
                              const AttributePtr& theFirstPointAttr,
                              const AttributePtr& theSecondPointAttr);

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
  /// \param theFirstAttribute an attribute of further coincidence
  std::shared_ptr<ModelAPI_Feature> createConstraintForObjects(const std::string& theConstraintId,
                        const std::shared_ptr<ModelAPI_Object>& theFirstObject,
                        const std::shared_ptr<ModelAPI_Object>& theSecondObject);

  /// Add feature coincidence constraint between given attributes
  /// \param theFeaturesToUpdate a constraint index
  void updateFeaturesAfterSplit(const std::set<FeaturePtr>& theFeaturesToUpdate);

  /// Result result of the feature to build constraint with. For arc, circle it is an edge result.
  /// \param theFeature a feature
  /// \return result object
  std::shared_ptr<ModelAPI_Result> getFeatureResult(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Returns attributes of the feature, used in edge build, for arc it is end and start points
  /// \param theFeature a feature
  /// \return container of attributes
  std::set<std::shared_ptr<ModelAPI_Attribute> > getEdgeAttributes(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Returns first attribute coincident to the first/second point of selected shape
  /// \param isFirstAttribute true for getting the first atribute, false otherwise
  /// \return an attribute or NULL
  std::shared_ptr<GeomDataAPI_Point2D> getPointAttribute(const bool isFirstAttribute);

#ifdef _DEBUG
  /// Return feature name, kind, point attribute values united in a string
  /// \param theFeature an investigated feature
  /// \return string value
  std::string getFeatureInfo(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const bool isUseAttributesInfo = true);
#endif
private:
  typedef std::map<std::shared_ptr<GeomDataAPI_Point2D>,
                   std::shared_ptr<GeomAPI_Pnt> > PntToAttributesMap;

  std::map<std::shared_ptr<ModelAPI_Object>, std::set<GeomShapePtr> > myCashedShapes;
  std::map<std::shared_ptr<ModelAPI_Object>, PntToAttributesMap> myCashedReferences;
};

#endif
