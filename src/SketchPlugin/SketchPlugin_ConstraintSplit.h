// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintSplit.h
// Created: 25 Aug 2016
// Author:  Natalia ERMOLAEVA

#ifndef SketchPlugin_ConstraintSplit_H_
#define SketchPlugin_ConstraintSplit_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

class GeomDataAPI_Point2D;
class ModelAPI_Feature;
class ModelAPI_Result;

typedef std::pair<std::string, std::shared_ptr<GeomDataAPI_Point2D> > IdToPointPair;

/** \class SketchPlugin_ConstraintSplit
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint splitting object. Entities for split:
 * - Linear segment by point(s) on this line
 * - Arc by point(s) on this arc
 * - Circle by at least 2 split-points on this circle
 *
 * The following constraints will be applied after split to keep the divided segments geometry:
 * - Coincident constraints for both parts of created segments in the point of splitting
 * - For linear segments parallel, for circles – tangent constraint, for arc – tangent and equal
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
class SketchPlugin_ConstraintSplit : public SketchPlugin_ConstraintBase
{
 public:
  /// Split constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_SPLIT_ID("SketchConstraintSplit");
    return MY_CONSTRAINT_SPLIT_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintSplit::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const;

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  /// This is necessary to perform execute only by apply the feature
  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintSplit();

  /// \return map of base points and features;
  //SKETCHPLUGIN_EXPORT const std::map<AttributePtr, FilletFeatures> pointsFeaturesMap() const {
  //  return myPointFeaturesMap;
  //};

private:
  /// Returns geom point attribute of the feature bounds. It processes line or arc.
  /// For circle feature, the result attributes are null
  /// \param theStartPointAttr an out attribute to start point
  /// \param theStartPointAttr an out attribute to end point
  void getFeaturePoints(std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                        std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr);

  /// Returns cast of attribute to geometrical point if the attribute is a ref attr attribute
  /// \param theAttribute an attribute
  /// \param geom point 2D or NULL
  std::shared_ptr<GeomDataAPI_Point2D> getPointOfRefAttr(const AttributePtr& theAttribute);

  /// Obtains those constraints of the feature that should be modified. output maps contain
  /// point of coincidence and attribute id to be modified after split
  /// \param theFeaturesToDelete [out] constrains that will be deleted after split
  /// \param theTangentFeatures  [out] tangent feature to be connected to new feature
  /// \param theCoincidenceToFeature [out] coincidence to feature to be connected to new feature
  /// \param theCoincidenceToPoint [out] coincidence to point be connected to new feature
  void getConstraints(std::set<std::shared_ptr<ModelAPI_Feature>>& theFeaturesToDelete,
              std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theTangentFeatures,
              std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToFeature,
              std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToPoint);

  /// Move coincidence constraint from feature to point if it is found
  /// \param theCoincidenceToFeature coincidence to feature to be connected to new feature
  /// \param theFurtherCoincidences a list of points where coincidences will be build
  /// \paramv theFeatureResults created results after split where constaint might be connected
  void updateCoincidenceConstraintsToFeature(
              const std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToFeature,
              const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences,
              const std::set<ResultPtr>& theFeatureResults);

  /// Move tangency constraint to the nearest split feature that has a coincidence to the tangent
  /// \param theTangentFeatures tangencies to feature to be connected to nearest feature
  /// \param theFurtherCoincidences a list of points where coincidences is built
  void updateTangentConstraintsToFeature(
              const std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theTangentFeatures,
              const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences);

  /// Make the base object is splitted by the point attributes
  /// \param theSplitFeature a result split feature
  /// \param theBeforeFeature a feature between start point and the 1st point of split feature
  /// \param theAfterFeature a feature between last point of split feature and the end point
  /// \param thePoints a list of points where coincidences will be build
  /// \param theCreatedFeatures a container of created features
  void splitLine(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
                 std::shared_ptr<ModelAPI_Feature>& theBeforeFeature,
                 std::shared_ptr<ModelAPI_Feature>& theAfterFeature,
                 std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                 std::set<std::shared_ptr<ModelAPI_Feature>>& theCreatedFeatures);

  /// Make the base object is splitted by the point attributes
  /// \param theSplitFeature a result split feature
  /// \param theBeforeFeature a feature between start point and the 1st point of split feature
  /// \param theAfterFeature a feature between last point of split feature and the end point
  /// \param thePoints a list of points where coincidences will be build
  /// \param theCreatedFeatures a container of created features
  void splitArc(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
                std::shared_ptr<ModelAPI_Feature>& theBeforeFeature,
                std::shared_ptr<ModelAPI_Feature>& theAfterFeature,
                std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                std::set<std::shared_ptr<ModelAPI_Feature>>& theCreatedFeatures);

  /// Make the base object is splitted by the point attributes
  /// \param theSplitFeature a result split feature
  /// \param theBeforeFeature a feature between start point and the 1st point of split feature
  /// \param theAfterFeature a feature between last point of split feature and the end point
  /// \param thePoints a list of points where coincidences will be build
  /// \param theCreatedFeatures a container of created features
  void splitCircle(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
                   std::shared_ptr<ModelAPI_Feature>& theBeforeFeature,
                   std::shared_ptr<ModelAPI_Feature>& theAfterFeature,
                   std::set<std::shared_ptr<GeomDataAPI_Point2D> >& thePoints,
                   std::set<std::shared_ptr<ModelAPI_Feature>>& theCreatedFeatures);

  /// Correct the first and the second point to provide condition that the first is closer to
  /// the start point and the second point - to the last end of current segment. To rearrange
  /// them if this condition is not satisfied.
  /// \param theStartPointAttr a start point of a segment
  /// \param theEndPointAttr an end point of a segment
  /// \param theFirstPointAttr a start point of a segment
  /// \param theSecondPointAttr an end point of a segment
  void arrangePoints(const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                     const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr,
                     std::shared_ptr<GeomDataAPI_Point2D>& theFirstPointAttr,
                     std::shared_ptr<GeomDataAPI_Point2D>& theSecondPointAttr);

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

  /// Return feature name, kind, point attribute values united in a string
  /// \param theFeature an investigated feature
  /// \return string value
  std::string getFeatureInfo(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const bool isUseAttributesInfo = true);
};

#endif
