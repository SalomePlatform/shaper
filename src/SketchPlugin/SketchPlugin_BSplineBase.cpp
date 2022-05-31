// Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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

#include <SketchPlugin_BSplineBase.h>

#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_MacroBSpline.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

#include <Events_InfoMessage.h>

#include <GeomAlgoAPI_EdgeBuilder.h>

#include <GeomAPI_BSpline2d.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>


SketchPlugin_BSplineBase::SketchPlugin_BSplineBase()
  : SketchPlugin_SketchEntity()
{
}

void SketchPlugin_BSplineBase::initDerivedClassAttributes()
{
  data()->addAttribute(POLES_ID(), GeomDataAPI_Point2DArray::typeId());
  data()->addAttribute(WEIGHTS_ID(), ModelAPI_AttributeDoubleArray::typeId());
  data()->addAttribute(KNOTS_ID(), ModelAPI_AttributeDoubleArray::typeId());
  data()->addAttribute(MULTS_ID(), ModelAPI_AttributeIntArray::typeId());
  data()->addAttribute(DEGREE_ID(), ModelAPI_AttributeInteger::typeId());

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_BSplineBase::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return;
  }

  AttributePoint2DArrayPtr aPolesArray =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POLES_ID()));
  AttributeDoubleArrayPtr aWeightsArray = data()->realArray(WEIGHTS_ID());
  AttributeDoubleArrayPtr aKnotsArray = data()->realArray(KNOTS_ID());
  AttributeIntArrayPtr aMultsArray = data()->intArray(MULTS_ID());
  AttributeIntegerPtr aDegreeAttr = data()->integer(DEGREE_ID());

  // collect poles
  std::list<GeomPnt2dPtr> aPoles2D;
  for (int anIndex = 0; anIndex < aPolesArray->size(); ++anIndex) {
    GeomPnt2dPtr aPole = aPolesArray->pnt(anIndex);
    aPoles2D.push_back(aPole);
  }
  // collect weights
  std::list<double> aWeights;
  for (int anIndex = 0; anIndex < aWeightsArray->size(); ++anIndex)
    aWeights.push_back(aWeightsArray->value(anIndex));
  // collect knots
  std::list<double> aKnots;
  for (int anIndex = 0; anIndex < aKnotsArray->size(); ++anIndex)
    aKnots.push_back(aKnotsArray->value(anIndex));
  // collect multiplicities
  std::list<int> aMults;
  for (int anIndex = 0; anIndex < aMultsArray->size(); ++anIndex)
    aMults.push_back(aMultsArray->value(anIndex));

  // create result B-spline curve
  GeomShapePtr anEdge = GeomAlgoAPI_EdgeBuilder::bsplineOnPlane(aSketch->coordinatePlane(),
      aPoles2D, aWeights, aKnots, aMults, aDegreeAttr->value(), isPeriodic());

  ResultConstructionPtr aResult = document()->createConstruction(data(), 0);
  aResult->setShape(anEdge);
  aResult->setIsInHistory(false);
  setResult(aResult, 0);
}

bool SketchPlugin_BSplineBase::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_BSplineBase::attributeChanged(const std::string& /*theID*/)
{
}

bool SketchPlugin_BSplineBase::customAction(const std::string& theActionId)
{
  // parse for the action and an index divided by '#'
  std::string anAction;
  int anIndex = -1;
  size_t pos = theActionId.find('#');
  if (pos != std::string::npos) {
    anAction = theActionId.substr(0, pos);
    anIndex = std::stoi(theActionId.substr(pos + 1));
  }

  if (anAction == ADD_POLE_ACTION_ID()) {
    return addPole(anIndex);
  }

  std::string aMsg = "Error: Feature \"%1\" does not support action \"%2\".";
  Events_InfoMessage("SketchPlugin_BSplineBase", aMsg).arg(getKind()).arg(theActionId).send();
  return false;
}

bool SketchPlugin_BSplineBase::addPole(const int theAfter)
{
  AttributePoint2DArrayPtr aPolesArray =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POLES_ID()));
  AttributeDoubleArrayPtr aWeightsArray = data()->realArray(WEIGHTS_ID());

  int anAfter = (!isPeriodic() && theAfter == aPolesArray->size() - 1) ? theAfter - 1 : theAfter;

  // find internal coincidences applied to the poles with greater indices
  std::list<AttributeIntegerPtr> aCoincidentPoleIndex;
  std::map<int, FeaturePtr> aControlPoles, aControlSegments;
  bool hasAuxSegment = false;
  const std::set<AttributePtr>& aRefs = data()->refsToMe();
  for (std::set<AttributePtr>::iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature((*anIt)->owner());
    if (aFeature->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID()) {
      AttributeIntegerPtr anIndex;
      AttributeRefAttrPtr aNonSplinePoint;
      if ((*anIt)->id() == SketchPlugin_ConstraintCoincidenceInternal::ENTITY_A()) {
        anIndex = aFeature->integer(SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_A());
        aNonSplinePoint = aFeature->refattr(SketchPlugin_Constraint::ENTITY_B());
      }
      else if ((*anIt)->id() == SketchPlugin_ConstraintCoincidenceInternal::ENTITY_B()) {
        anIndex = aFeature->integer(SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_B());
        aNonSplinePoint = aFeature->refattr(SketchPlugin_Constraint::ENTITY_A());
      }

      if (anIndex && anIndex->isInitialized()) {
        if (anIndex->value() > anAfter) {
          aCoincidentPoleIndex.push_back(anIndex);
          FeaturePtr aParent = ModelAPI_Feature::feature(aNonSplinePoint->attr()->owner());
          if (aParent->getKind() == SketchPlugin_Point::ID())
            aControlPoles[anIndex->value()] = aParent;
          else if (aParent->getKind() == SketchPlugin_Line::ID() &&
                   aNonSplinePoint->attr()->id() == SketchPlugin_Line::START_ID())
            aControlSegments[anIndex->value()] = aParent;
        }
        else if (anIndex->value() == anAfter && !hasAuxSegment) {
          // check the constrained object is a segment of the control polygon
          if (aNonSplinePoint && !aNonSplinePoint->isObject() &&
              aNonSplinePoint->attr()->id() == SketchPlugin_Line::START_ID()) {
            hasAuxSegment = true;
            aCoincidentPoleIndex.push_back(anIndex);
            aControlSegments[anIndex->value()] =
                ModelAPI_Feature::feature(aNonSplinePoint->attr()->owner());
          }
        }
      }
    }
  }

  bool aWasBlocked = data()->blockSendAttributeUpdated(true);

  // add new pole and default weight
  std::list<GeomPnt2dPtr> aPoles;
  aPolesArray->setSize(aPolesArray->size() + 1);
  aPolesArray->setPnt(aPolesArray->size() - 1, aPolesArray->pnt(0)); // for periodic spline
  for (int i = aPolesArray->size() - 2; i > anAfter; --i) {
    aPoles.push_front(aPolesArray->pnt(i));
    aPolesArray->setPnt(i + 1, aPoles.front());
  }

  GeomPnt2dPtr aCurPole = aPolesArray->pnt(anAfter);
  GeomPnt2dPtr aNextPole = aPolesArray->pnt(anAfter + 1);
  aPolesArray->setPnt(anAfter + 1, (aCurPole->x() + aNextPole->x()) * 0.5,
                                   (aCurPole->y() + aNextPole->y()) * 0.5);
  for (int i = anAfter + 1; i >= 0; --i)
    aPoles.push_front(aPolesArray->pnt(i));

  std::list<double> aWeights;
  for (int i = 0; i < aWeightsArray->size(); ++i) {
    aWeights.push_back(aWeightsArray->value(i));
    if (i == anAfter)
      aWeights.push_back(1.0); // default weight
  }
  aWeightsArray->setSize(aWeightsArray->size() + 1);
  std::list<double>::iterator aWIt = aWeights.begin();
  for (int i = 0; i < aWeightsArray->size(); ++i, ++aWIt)
    aWeightsArray->setValue(i, *aWIt);

  // recalculate knots and multiplicities
  std::shared_ptr<GeomAPI_BSpline2d> aBSplineCurve(
      new GeomAPI_BSpline2d(aPoles, aWeights, isPeriodic()));

  integer(DEGREE_ID())->setValue(aBSplineCurve->degree());

  AttributeDoubleArrayPtr aKnotsAttr = data()->realArray(SketchPlugin_BSplineBase::KNOTS_ID());
  std::list<double> aKnots = aBSplineCurve->knots();
  int aSize = (int)aKnots.size();
  aKnotsAttr->setSize(aSize);
  std::list<double>::iterator aKIt = aKnots.begin();
  for (int index = 0; index < aSize; ++index, ++aKIt)
    aKnotsAttr->setValue(index, *aKIt);

  AttributeIntArrayPtr aMultsAttr = data()->intArray(SketchPlugin_BSplineBase::MULTS_ID());
  std::list<int> aMults = aBSplineCurve->mults();
  aSize = (int)aMults.size();
  aMultsAttr->setSize(aSize);
  std::list<int>::iterator aMIt = aMults.begin();
  for (int index = 0; index < aSize; ++index, ++aMIt)
    aMultsAttr->setValue(index, *aMIt);

  data()->blockSendAttributeUpdated(aWasBlocked, true);

  // update indices of internal coincidences
  for (std::list<AttributeIntegerPtr>::iterator aCIt = aCoincidentPoleIndex.begin();
       aCIt != aCoincidentPoleIndex.end(); ++aCIt)
    (*aCIt)->setValue((*aCIt)->value() + 1);

  // create auxiliary segment and pole updating the control polygon
  SketchPlugin_MacroBSpline::createAuxiliaryPole(aPolesArray, anAfter + 1);
  if (hasAuxSegment)
    SketchPlugin_MacroBSpline::createAuxiliarySegment(aPolesArray, anAfter, anAfter + 1);

  // update names of features representing control polygon
  for (std::map<int, FeaturePtr>::iterator anIt = aControlPoles.begin();
       anIt != aControlPoles.end(); ++anIt) {
    SketchPlugin_MacroBSpline::assignDefaultNameForAux(anIt->second, aPolesArray, anIt->first + 1);
  }
  for (std::map<int, FeaturePtr>::iterator anIt = aControlSegments.begin();
       anIt != aControlSegments.end(); ++anIt) {
    SketchPlugin_MacroBSpline::assignDefaultNameForAux(anIt->second, aPolesArray,
        anIt->first + 1, (anIt->first + 2) % aPolesArray->size());
  }

  return true;
}
