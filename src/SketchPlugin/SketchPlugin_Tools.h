// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Tools.h
// Created:     07 July 2015
// Author:      Sergey POKHODENKO

#ifndef SKETCHPLUGIN_TOOLS_H_
#define SKETCHPLUGIN_TOOLS_H_

#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <GeomDataAPI_Point2D.h>

class SketchPlugin_Feature;

namespace SketchPlugin_Tools {

/// Clears text expressions for all attributes of the feature
void clearExpressions(FeaturePtr theFeature);

/// \return coincidence point
/// \param[in] theStartCoin coincidence feature
std::shared_ptr<GeomAPI_Pnt2d> getCoincidencePoint(const FeaturePtr theStartCoin);

/// Find all Coincident constraints referred to the feature or its attribute
std::set<FeaturePtr> findCoincidentConstraints(const FeaturePtr& theFeature);

/// Finds lines coincident at point
/// \param[in] theStartCoin coincidence feature
/// \param[in] theAttr attribute name
/// \param[out] theList list of lines
void findCoincidences(const FeaturePtr theStartCoin,
                      const std::string& theAttr,
                      std::set<FeaturePtr>& theList);

/// Find all features the point is coincident to.
std::set<FeaturePtr> findFeaturesCoincidentToPoint(const AttributePoint2DPtr& thePoint);

/// Find all points the given point is coincident to.
std::set<AttributePoint2DPtr> findPointsCoincidentToPoint(const AttributePoint2DPtr& thePoint);

void resetAttribute(SketchPlugin_Feature* theFeature, const std::string& theId);

/// Creates coincidence or tangent constraint.
/// \param[in] theFeature to get selected attribute or object
/// \param[in] theId ID of attribute where selection is.
/// \param[in] theObject object for constraint
/// \param[in] theIsCanBeTangent if true constraint can be tangent or coincidence, depending on
///                              the selection in the attribute with passed ID.
void createConstraint(SketchPlugin_Feature* theFeature,
                      const std::string& theId,
                      const AttributePtr theAttr,
                      const ObjectPtr theObject,
                      const bool theIsCanBeTangent);

/// Creates passing point or tangent curve basing on the given attributes are initialized.
/// \param[in]  theRefAttr       prefered attribute to be converted
/// \param[in]  theDefaultAttr   default attribute if theRefAttr is not initialized
/// \param[out] theTangentCurve  tangent curve if theRefAttr refers to an edge
/// \param[out] thePassingPoint  passing point if theRefAttr does not refer to an edge
void convertRefAttrToPointOrTangentCurve(const AttributeRefAttrPtr&      theRefAttr,
                                         const AttributePtr&             theDefaultAttr,
                                         std::shared_ptr<GeomAPI_Shape>& theTangentCurve,
                                         std::shared_ptr<GeomAPI_Pnt2d>& thePassingPoint);
}; // namespace SketchPlugin_Tools

#endif // SKETCHPLUGIN_TOOLS_H_