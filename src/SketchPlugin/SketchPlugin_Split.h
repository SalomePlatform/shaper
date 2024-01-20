// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef SketchPlugin_Split_H_
#define SketchPlugin_Split_H_

#include "SketchPlugin.h"
#include "SketchPlugin_Tools.h"

#include <GeomAPI_IPresentable.h>
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
 * - Circular/elliptic arc by point(s) on this arc
 * - Circle/ellipse by at least 2 split-points on it
 *
 * The following constraints will be applied after split to keep the divided segments geometry:
 * - Coincident constraints for both parts of created segments in the point of splitting
 * - For linear segments parallel, for circles/ellipses - tangent constraint,
 *   for arc - tangent and equal constraints. In case of three segments in result
 *   two couple of constraints are created
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

  /// Apply information of the message to current object. It fills selected point and object
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage);

private:
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

  /// Move coincidence constraint from feature to point if it is found
  /// \param theCoincidenceToFeature coincidence to feature to be connected to new feature
  /// \param theFurtherCoincidences a list of points where coincidences will be build
  /// \param theFeatureResults created results after split where constaint might be connected
  /// \param theSplitFeature feature created by split, new coincidences to points should be created
  /// \param theFeaturesToDelete the list of removed features (will be updated here by
  ///                            the coincidences to be removed)
  /// if theCoincidenceToFeature contains equal points
  void updateCoincidenceConstraintsToFeature(
      const std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToFeature,
      const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences,
      const std::set<ResultPtr>& theFeatureResults,
      const FeaturePtr& theSplitFeature,
      std::set<FeaturePtr>& theFeaturesToDelete);

  /// Move constraints from base feature to given feature
  /// \param theFeature a base feature
  /// \param theRefsToFeature list of attributes referenced to base feature
  void updateRefFeatureConstraints(const std::shared_ptr<ModelAPI_Result>& theFeatureBaseResult,
                                   const std::list<AttributePtr>& theRefsToFeature);

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
  /// \return new elliptic arc if it was created
  FeaturePtr splitEllipticArc(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
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
  FeaturePtr splitClosed(std::shared_ptr<ModelAPI_Feature>& theSplitFeature,
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

  std::map<std::shared_ptr<ModelAPI_Object>, std::set<GeomShapePtr> > myCashedShapes;
  std::map<std::shared_ptr<ModelAPI_Object>,
           GeomAlgoAPI_ShapeTools::PointToRefsMap> myCashedReferences;
};

#endif
