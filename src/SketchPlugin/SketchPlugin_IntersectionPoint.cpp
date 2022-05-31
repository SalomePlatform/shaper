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

#include "SketchPlugin_IntersectionPoint.h"
#include "SketchPlugin_Point.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>

SketchPlugin_IntersectionPoint::SketchPlugin_IntersectionPoint()
  : SketchPlugin_SketchEntity(),
    myIsComputing(false)
{
}

void SketchPlugin_IntersectionPoint::initDerivedClassAttributes()
{
  data()->addAttribute(EXTERNAL_FEATURE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(INTERSECTION_POINTS_ID(), ModelAPI_AttributeRefList::typeId());
  data()->attribute(INTERSECTION_POINTS_ID())->setIsArgument(false);

  data()->addAttribute(INCLUDE_INTO_RESULT(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), AUXILIARY_ID());
}

void SketchPlugin_IntersectionPoint::execute()
{
  AttributeRefListPtr anIntersectionsList = reflist(INTERSECTION_POINTS_ID());
  if (!anIntersectionsList || !anIntersectionsList->isInitialized())
    return; // no intersections

  computePoint(EXTERNAL_FEATURE_ID());
}

void SketchPlugin_IntersectionPoint::attributeChanged(const std::string& theID)
{
  // compute intersection between line and sketch plane
  computePoint(theID);
}

void SketchPlugin_IntersectionPoint::computePoint(const std::string& theID)
{
  if (theID != EXTERNAL_FEATURE_ID() && theID != EXTERNAL_ID())
    return;

  if (myIsComputing)
    return;
  myIsComputing = true;

  AttributeSelectionPtr anExternalFeature = selection(EXTERNAL_FEATURE_ID());

  GeomShapePtr aShape;
  GeomEdgePtr anEdge;
  if (anExternalFeature)
    aShape = anExternalFeature->value();
  if (!aShape && anExternalFeature->context())
    aShape = anExternalFeature->context()->shape();
  if (aShape && aShape->isEdge())
    anEdge = GeomEdgePtr(new GeomAPI_Edge(aShape));

  if (anEdge) {
    std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

    std::list<GeomPointPtr> anIntersectionsPoints;
    anEdge->intersectWithPlane(aSketchPlane, anIntersectionsPoints);

    AttributeRefListPtr anIntersectionsList = reflist(INTERSECTION_POINTS_ID());
    std::list<ObjectPtr> anExistentIntersections = anIntersectionsList->list();
    std::list<ObjectPtr>::const_iterator aExistInterIt = anExistentIntersections.begin();

    const std::list<ResultPtr>& aResults = results();
    std::list<ResultPtr>::const_iterator aResIt = aResults.begin();

    int aResultIndex = 0;
    for (std::list<GeomPointPtr>::iterator aPntIt = anIntersectionsPoints.begin();
         aPntIt != anIntersectionsPoints.end(); ++aPntIt, ++aResultIndex) {
      std::shared_ptr<SketchPlugin_Point> aCurSketchPoint;
      if (aExistInterIt == anExistentIntersections.end()) {
        keepCurrentFeature();

        // create new point
        aCurSketchPoint = std::dynamic_pointer_cast<SketchPlugin_Point>(
          sketch()->addFeature(SketchPlugin_Point::ID()));
        aCurSketchPoint->boolean(COPY_ID())->setValue(true);
        anIntersectionsList->append(aCurSketchPoint);

        restoreCurrentFeature();
      } else {
        // update existent point
        aCurSketchPoint = std::dynamic_pointer_cast<SketchPlugin_Point>(*aExistInterIt);
        ++aExistInterIt;
      }

      ResultConstructionPtr aCurResult;
      if (aResIt == aResults.end()) {
        // create new result
        aCurResult = document()->createConstruction(data(), aResultIndex);
        aCurResult->setIsInHistory(false);
        aCurResult->setDisplayed(false);
      } else {
        // update existent result
        aCurResult = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aResIt);
        aCurResult->setShape(std::shared_ptr<GeomAPI_Edge>());
        ++aResIt;
      }

      // update coordinates of intersection
      GeomPnt2dPtr aPointInSketch = sketch()->to2D(*aPntIt);
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCurSketchPoint->attribute(SketchPlugin_Point::COORD_ID()))->setValue(aPointInSketch);
      aCurSketchPoint->execute();

      // update result
      aCurResult->setShape(aCurSketchPoint->lastResult()->shape());
      setResult(aCurResult, aResultIndex);

      // make intersection point external
      GeomShapePtr anEmptyVal;
      aCurSketchPoint->selection(EXTERNAL_ID())->setValue(aCurResult, anEmptyVal);
    }

    // remove rest results from previous pass
    removeResults(aResultIndex);
    std::set<FeaturePtr> aFeaturesToBeRemoved;
    for (; aExistInterIt != anExistentIntersections.end(); ++aExistInterIt) {
      aFeaturesToBeRemoved.insert(ModelAPI_Feature::feature(*aExistInterIt));
      anIntersectionsList->removeLast();
    }
    ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);

    if (theID != EXTERNAL_ID())
      selection(EXTERNAL_ID())->selectValue(anExternalFeature);
  }
  myIsComputing = false;
}
