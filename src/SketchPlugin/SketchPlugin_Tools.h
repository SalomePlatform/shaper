// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SKETCHPLUGIN_TOOLS_H_
#define SKETCHPLUGIN_TOOLS_H_

#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <list>
#include <map>

class GeomAPI_AISObject;

class SketchPlugin_Constraint;
class SketchPlugin_Feature;
class SketchPlugin_Sketch;

namespace SketchPlugin_Tools {

/// Clears text expressions for all attributes of the feature
void clearExpressions(FeaturePtr theFeature);

/// \return coincidence point
/// \param[in] theStartCoin coincidence feature
std::shared_ptr<GeomAPI_Pnt2d> getCoincidencePoint(const FeaturePtr theStartCoin);

/// Find all Coincident constraints referred to the feature or its attribute
std::set<FeaturePtr> findCoincidentConstraints(const ObjectPtr& theObject);

/// Finds lines coincident at point
/// \param[in] theStartCoin coincidence feature
/// \param[in] theAttr attribute name
/// \param[out] theList list of lines
/// \param[in] theIsAttrOnly if true includes only coincidences with attributes.
void findCoincidences(const FeaturePtr theStartCoin,
                      const std::string& theAttr,
                      std::set<FeaturePtr>& theList,
                      const bool theIsAttrOnly = false);

/// Find all features the point is coincident to.
std::set<FeaturePtr> findFeaturesCoincidentToPoint(const AttributePoint2DPtr& thePoint);

/// Find all points the given point is coincident to.
std::set<AttributePoint2DPtr> findPointsCoincidentToPoint(const AttributePoint2DPtr& thePoint);

void resetAttribute(SketchPlugin_Feature* theFeature, const std::string& theId);

/// Create new constraint between given attributes
/// \param[in] theSketch          a sketch where the constraint will be created
/// \param[in] theConstraintId    a constraint identifier
/// \param[in] theFirstAttribute  an attribute of further constraint
/// \param[in] theSecondAttribute an attribute of further constraint
FeaturePtr createConstraintAttrAttr(SketchPlugin_Sketch* theSketch,
                                    const std::string& theConstraintId,
                                    const AttributePtr& theFirstAttribute,
                                    const AttributePtr& theSecondAttribute);

/// Create new constraint between given attribute and object
/// \param[in] theSketch         a sketch where the constraint will be created
/// \param[in] theConstraintId   a constraint identifier
/// \param[in] theFirstAttribute an attribute of further constraint
/// \param[in] theSecondObject   an attribute of further constraint
FeaturePtr createConstraintAttrObject(SketchPlugin_Sketch* theSketch,
                                      const std::string& theConstraintId,
                                      const AttributePtr& theFirstAttribute,
                                      const ObjectPtr& theSecondObject);

/// Create new constraint between given objects
/// \param[in] theSketch       a sketch where the constraint will be created
/// \param[in] theConstraintId a constraint identifier
/// \param[in] theFirstObject  an attribute of further constraint
/// \param[in] theSecondObject an attribute of further constraint
FeaturePtr createConstraintObjectObject(SketchPlugin_Sketch* theSketch,
                                        const std::string& theConstraintId,
                                        const ObjectPtr& theFirstObject,
                                        const ObjectPtr& theSecondObject);

/// Creates coincidence or tangent constraint.
/// \param[in] theFeature to get selected attribute or object
/// \param[in] theId ID of attribute where selection is.
/// \param[in] theObject object for constraint
/// \param[in] theIsCanBeTangent if true constraint can be tangent or coincidence, depending on
///                              the selection in the attribute with passed ID.
void createCoincidenceOrTangency(SketchPlugin_Feature* theFeature,
                                 const std::string& theId,
                                 const AttributePtr theAttr,
                                 const ObjectPtr theObject,
                                 const bool theIsCanBeTangent);

/// Creates auxiliary point for ellipse and corresponding internal constraint.
/// \param[in] theEllipse   base ellipse feature
/// \param[in] theAttrName  name of the attribute of the ellipse,
///                         the new point should be constrained
void createAuxiliaryPointOnEllipse(const FeaturePtr& theEllipseFeature,
                                   const std::string& theAttrName);

/// Creates auxiliary axis for ellipse and corresponding internal constraints.
/// \param[in] theEllipse   base ellipse feature
/// \param[in] theStartAttr name of the attribute of the ellipse, the line is started
/// \param[in] theEndAttr   name of the attribute of the ellipse, the line is ended
void createAuxiliaryAxisOfEllipse(const FeaturePtr& theEllipseFeature,
                                  const std::string& theStartAttr,
                                  const std::string& theEndAttr);

/// Creates passing point or tangent curve basing on the given attributes are initialized.
/// \param[in]  theRefAttr       prefered attribute to be converted
/// \param[in]  theDefaultAttr   default attribute if theRefAttr is not initialized
/// \param[out] theTangentCurve  tangent curve if theRefAttr refers to an edge
/// \param[out] thePassingPoint  passing point if theRefAttr does not refer to an edge
void convertRefAttrToPointOrTangentCurve(const AttributeRefAttrPtr&      theRefAttr,
                                         const AttributePtr&             theDefaultAttr,
                                         std::shared_ptr<GeomAPI_Shape>& theTangentCurve,
                                         std::shared_ptr<GeomAPI_Pnt2d>& thePassingPoint);

/// Calculate global coordinates for flyout point of Length constraint
GeomPnt2dPtr flyoutPointCoordinates(const std::shared_ptr<SketchPlugin_Constraint>& theConstraint);

/// Sets attributes of feature presentation
/// \param[in] thePrs a presentation
/// \param[in] isAxiliary is axiliary flag
void customizeFeaturePrs(const AISObjectPtr& thePrs, bool isAxiliary);

void setDimensionColor(const AISObjectPtr& theDimPrs);

/// Replace string in the name of object
void replaceInName(ObjectPtr theObject, const std::wstring& theSource, const std::wstring& theDest);

}; // namespace SketchPlugin_Tools

namespace SketchPlugin_SegmentationTools
{
  /// Returns geom point attribute of the feature bounds. It processes line or arc.
  /// For circle/ellipse feature, the result attributes are null
  /// \param theFeature        a source feature
  /// \param theStartPointAttr an out attribute to start point
  /// \param theEndPointAttr   an out attribute to end point
  void getFeaturePoints(const FeaturePtr& theFeature,
                        std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                        std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr);

  /// Obtains references to feature point attributes and to feature,
  /// e.g. for feature line: 1st container is
  ///             <1st line point, list<entity_a in distance, entity_b in parallel> >
  ///             <2nd line point, list<> >
  ///      for feature circle 2nd container is <entity_a in Radius, entity_b in equal, ...>
  /// \param theFeature an investigated feature
  /// \param theRefs a container of list of referenced attributes
  /// \param theRefsToFeature references to the feature result
  void getRefAttributes(const FeaturePtr& theFeature,
                        std::map<AttributePtr, std::list<AttributePtr> >& theRefs,
                        std::list<AttributePtr>& theRefsToFeature);

  /// Obtains a part of shape selected/highlighted in the viewer for Split/Trim operation
  /// \param[in] theFeature            Split/Trim feature
  /// \param[in] theObjectAttributeId  name of attribute containing selected object
  /// \param[in] thePointAttributeId   name of attribute containing point selected on the object
  GeomShapePtr getSubShape(
      SketchPlugin_Feature* theFeature,
      const std::string& theObjectAttributeId,
      const std::string& thePointAttributeId,
      std::map<ObjectPtr, std::set<GeomShapePtr> >& theCashedShapes,
      std::map<ObjectPtr, GeomAlgoAPI_ShapeTools::PointToRefsMap>& theObjectToPoints);

  /// Fulfill an internal containers by shapes obtained from the parameter object
  /// Shapes are results of Split/Trim operation by points coincident to shape of the object
  /// \param theOpFeture an operation feature (Split/Trim)
  /// \param theObject a source object (will be splitted)
  void fillObjectShapes(
      SketchPlugin_Feature* theOpFeature,
      const ObjectPtr& theObject,
      std::map<ObjectPtr, std::set<GeomShapePtr> >& theCashedShapes,
      std::map<ObjectPtr, GeomAlgoAPI_ShapeTools::PointToRefsMap>& theObjectToPoints);

  /// AIS object for selected/highlighted part of splitting/triming feature
  /// \param[in] thePrevious  previous presentation
  /// \param[in] theOpFeture  an operation feature (Split/Trim)
  std::shared_ptr<GeomAPI_AISObject> getAISObject(std::shared_ptr<GeomAPI_AISObject> thePrevious,
                                                  SketchPlugin_Feature* theOpFeature,
                                                  const std::string& thePreviewObjectAttrName,
                                                  const std::string& thePreviewPointAttrName,
                                                  const std::string& theSelectedObjectAttrName,
                                                  const std::string& theSelectedPointAttrName);

  /// Move constraints from attribute of base feature to attribute after modification
  /// \param theBaseRefAttributes container of references to the attributes of base feature
  /// \param theModifiedAttributes container of attributes placed instead of base attributes
  /// at the same place
  void updateRefAttConstraints(
      const std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
      const std::set<std::pair<AttributePtr, AttributePtr> >& theModifiedAttributes);

  /// Updates line length if it exist in the list
  /// \param theFeaturesToUpdate a constraint index
  void updateFeaturesAfterOperation(const std::set<FeaturePtr>& theFeaturesToUpdate);


  /// Creates a line feature filled by center of base feature and given points
  /// \param theBaseFeature another arc feature
  /// \param theFirstAttribute an attribute with coordinates for the start point
  /// \param theSecondAttribute an attribute with coordinates for the end point
  FeaturePtr createLineFeature(const FeaturePtr& theBaseFeature,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint);

  /// Creates a circular/elliptic arc feature filled by center
  /// (or by center and focus for elliptic arc) of base feature and given points
  /// \param theBaseFeature     another circle or ellipse or circular/elliptic arc
  /// \param theFirstAttribute  an attribute with coordinates for the start point
  /// \param theSecondAttribute an attribute with coordinates for the end point
  FeaturePtr createArcFeature(
      const FeaturePtr& theBaseFeature,
      const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
      const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint);

}; // namespace SketchPlugin_SegmentationTools

#endif // SKETCHPLUGIN_TOOLS_H_