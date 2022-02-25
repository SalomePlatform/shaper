// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <SketchPlugin_OverConstraintsResolver.h>

#include <Events_Loop.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_Point.h>

static SketchPlugin_OverConstraintsResolver* myConstResolver =
                                                   new SketchPlugin_OverConstraintsResolver;

SketchPlugin_OverConstraintsResolver::SketchPlugin_OverConstraintsResolver()
{
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_CHECK_CONSTRAINTS));
}

void SketchPlugin_OverConstraintsResolver::setConstraints(
                                           const std::set<ObjectPtr>& theConstraints)
{
  myConstraints = theConstraints;
}

bool SketchPlugin_OverConstraintsResolver::perform()
{
  bool hasConflicts = false;
  hasConflicts |= checkArcsAboutTangentialConflict();
  hasConflicts |= checkHorizontalOrVerticalConflict();
  return hasConflicts;
}

bool SketchPlugin_OverConstraintsResolver::checkHorizontalOrVerticalConflict()
{
  std::set<ObjectPtr>::const_iterator
    anIt = myConstraints.begin(), aLast = myConstraints.end();
  bool isHVConstraint = false;
  FeaturePtr aFeature;
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (!isHVConstraint) {
      aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
      if (aFeature) {
        std::string aType = aFeature->getKind();
        if ((aType == SketchPlugin_ConstraintHorizontal::ID()) ||
            (aType == SketchPlugin_ConstraintVertical::ID()))
        {
          myConstraintsToRemove.insert(*anIt);
          isHVConstraint = true;
        }
      }
    }
  }
  return isHVConstraint;
}

bool SketchPlugin_OverConstraintsResolver::checkArcsAboutTangentialConflict()
{
  bool isConflictsFound = false;

  std::set<ObjectPtr>::const_iterator
    anIt = myConstraints.begin(), aLast = myConstraints.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    ConstraintPtr aConstain =
      std::dynamic_pointer_cast<SketchPlugin_Constraint>(anObject);
    if (aConstain.get()) {
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttrA = aConstain->refattr(
                                                             aConstain->ENTITY_A());
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttrB = aConstain->refattr(
                                                             aConstain->ENTITY_B());
      if (!aRefAttrA || !aRefAttrB)
        continue;

      FeaturePtr aFeatureA = ModelAPI_Feature::feature(aRefAttrA->object());
      FeaturePtr aFeatureB = ModelAPI_Feature::feature(aRefAttrB->object());

      if (aFeatureA && aFeatureA->getKind() == SketchPlugin_Arc::ID() &&
          aFeatureB && aFeatureB->getKind() == SketchPlugin_Arc::ID()) {

        std::shared_ptr<ModelAPI_Attribute> anAttrStrA = aFeatureA->attribute(
                                                         SketchPlugin_Arc::START_ID());
        std::shared_ptr<ModelAPI_Attribute> anAttrEndA = aFeatureA->attribute(
                                                         SketchPlugin_Arc::END_ID());
        std::shared_ptr<ModelAPI_Attribute> anAttrStrB = aFeatureB->attribute(
                                                         SketchPlugin_Arc::START_ID());
        std::shared_ptr<ModelAPI_Attribute> anAttrEndB = aFeatureB->attribute(
                                                         SketchPlugin_Arc::END_ID());
        std::shared_ptr<ModelAPI_Attribute> anAttrCenA = aFeatureA->attribute(
                                                         SketchPlugin_Arc::CENTER_ID());
        std::shared_ptr<ModelAPI_Attribute> anAttrCenB = aFeatureB->attribute(
                                                         SketchPlugin_Arc::CENTER_ID());

        bool isCoincident = false;
        bool isCoincidentAtCenter = false;
        std::set<FeaturePtr> aTangentConstraints;

        std::set<FeaturePtr> aFeatures;
        std::map<FeaturePtr, std::set<FeaturePtr> > aFeaturesMap;
        aFeatures.insert(aFeatureA);
        aFeatures.insert(aFeatureB);
        ModelAPI_Tools::findAllReferences(aFeatures, aFeaturesMap);

        std::set<FeaturePtr> aFeaturesA = aFeaturesMap[aFeatureA];
        std::set<FeaturePtr> aFeaturesB = aFeaturesMap[aFeatureB];

        for (auto aFeatIter = aFeaturesA.begin(); aFeatIter != aFeaturesA.end(); ++aFeatIter) {
          if (aFeaturesB.find(aFeatIter.operator*()) != aFeaturesB.end()){
            const std::string& aType = (*aFeatIter)->getKind();
            if (aType == SketchPlugin_ConstraintCoincidence::ID()) {
              ConstraintPtr aCoincidence =
                  std::dynamic_pointer_cast<SketchPlugin_ConstraintCoincidence>(*aFeatIter);
              std::set<AttributePtr> anAttrSet;
              anAttrSet.insert(aCoincidence->refattr(aCoincidence->ENTITY_A())->attr());
              anAttrSet.insert(aCoincidence->refattr(aCoincidence->ENTITY_B())->attr());
              isCoincident |= ((anAttrSet.find(anAttrStrA) != anAttrSet.end() ||
                                anAttrSet.find(anAttrEndA) != anAttrSet.end()) &&
                               (anAttrSet.find(anAttrStrB) != anAttrSet.end() ||
                                anAttrSet.find(anAttrEndB) != anAttrSet.end()));
              isCoincidentAtCenter |= (anAttrSet.find(anAttrCenA) != anAttrSet.end() &&
                                       anAttrSet.find(anAttrCenB) != anAttrSet.end());
            }
            else if (aType == SketchPlugin_ConstraintTangent::ID()) {
              aTangentConstraints.insert(*aFeatIter);
            }
          }
        }

        if (isCoincident && isCoincidentAtCenter && !aTangentConstraints.empty()) {
          isConflictsFound = true;
          myConstraintsToRemove.insert(aTangentConstraints.begin(), aTangentConstraints.end());
        }
      }
    }
  }

  return isConflictsFound;
}

void SketchPlugin_OverConstraintsResolver::processEvent(
                                           const std::shared_ptr<Events_Message>& theMessage)
{
  Events_ID anEventID = theMessage->eventID();
  if (anEventID == Events_Loop::eventByName(EVENT_CHECK_CONSTRAINTS)) {
    std::shared_ptr<ModelAPI_CheckConstraintsMessage> aConstraintsMsg =
      std::dynamic_pointer_cast<ModelAPI_CheckConstraintsMessage>(theMessage);
    if (aConstraintsMsg.get()) {
      myConstraintsToRemove.clear();
      myConstraints = aConstraintsMsg->constraints();
      if (perform())
      {
        std::shared_ptr<ModelAPI_CheckConstraintsMessage> aMessage =
          std::shared_ptr<ModelAPI_CheckConstraintsMessage>(
            new ModelAPI_CheckConstraintsMessage(
              Events_Loop::eventByName(EVENT_REMOVE_CONSTRAINTS)));
        aMessage->setConstraints(myConstraintsToRemove);
        Events_Loop::loop()->send(aMessage);
      }
    }
  }
}
