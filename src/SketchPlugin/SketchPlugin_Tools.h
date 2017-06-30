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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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