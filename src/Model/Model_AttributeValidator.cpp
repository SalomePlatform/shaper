// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "Model_AttributeValidator.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

bool Model_AttributeValidator::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments,
                                       Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr anAttribue =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
    if (!anAttribue->expressionError().empty()) {
      theError = "Expression error: %1";
      theError.arg(anAttribue->expressionError());
      return false;
    }
  } else
  if (theAttribute->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr anAttribue =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    if (!anAttribue->expressionError().empty()) {
      theError = "Expression error: %1";
      theError.arg(anAttribue->expressionError());
      return false;
    }
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point::typeId()) {
    AttributePointPtr anAttribue =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);

    const char* aComponent[] = {"X", "Y", "Z"};
    std::string anErrorMessage;
    for (int i = 0; i < 3; ++i) {
      if (!anAttribue->expressionError(i).empty())
        anErrorMessage.append("\n").append(aComponent[i])
          .append(": ").append(anAttribue->expressionError(i));
    }
    if (!anErrorMessage.empty()) {
      theError = "Expression error: %1";
      theError.arg(anErrorMessage);
      return false;
    }
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr anAttribue =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);

    const char* aComponent[] = {"X", "Y"};
    std::string anErrorMessage;
    for (int i = 0; i < 2; ++i) {
      if (!anAttribue->expressionError(i).empty())
        anErrorMessage.append("\n").append(aComponent[i])
          .append(": ").append(anAttribue->expressionError(i));
    }
    if (!anErrorMessage.empty()) {
      theError = "Expression error: %1";
      theError.arg(anErrorMessage);
      return false;
    }
  } else { // #2903 : check that concealed attribute refers to already concealed result
    FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());

    std::set<ObjectPtr> alreadyProcessed; // optimization
    if (aFeat.get() &&
        ModelAPI_Session::get()->validators()->isConcealed(aFeat->getKind(), theAttribute->id())) {
      std::list<std::pair<std::string, std::list<ObjectPtr> > > allRefs;
      aFeat->data()->referencesToObjects(allRefs);
      std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator anIter = allRefs.begin();
      for(; anIter != allRefs.end(); anIter++) {
        if (anIter->first == theAttribute->id()) {
          const std::list<ObjectPtr>& aReferencedList = anIter->second;
          std::list<ObjectPtr>::const_iterator aRefIter = aReferencedList.cbegin();
          for(; aRefIter != aReferencedList.cend(); aRefIter++) {
            const ObjectPtr& aReferenced = *aRefIter;
            if (!aReferenced.get())
              continue;
            // get all results and feature that is referenced to see all references to them
            FeaturePtr aReferencedFeature;
            if (aReferenced->groupName() == ModelAPI_Feature::group()) {
              aReferencedFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aReferenced);
            } else {
              aReferencedFeature = aReferenced->document()->feature(
                std::dynamic_pointer_cast<ModelAPI_Result>(aReferenced));
            }
            if (alreadyProcessed.count(aReferencedFeature))
              continue;
            alreadyProcessed.insert(aReferencedFeature);
            /* it takes all results, not only concealed
            std::list<ResultPtr> aReferencedResults;
            ModelAPI_Tools::allResults(aReferencedFeature, aReferencedResults);
            */
            std::list<ResultPtr> aReferencedResults;
            ResultBodyPtr aRefBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aReferenced);
            if (aRefBody.get()) { // take only sub-results of this result or sub-result
              ResultBodyPtr aRoot = ModelAPI_Tools::bodyOwner(aRefBody, true);
              if (aRoot.get()) {
                ModelAPI_Tools::allSubs(aRoot, aReferencedResults, false);
                aReferencedResults.push_back(aRoot);
              } else
                aReferencedResults.push_back(aRefBody);
            }

            std::list<ResultPtr>::iterator aRefRes = aReferencedResults.begin();
            bool aCheckFeature = true; // the last iteration to check the feature
            while(aRefRes != aReferencedResults.end() || aCheckFeature) {
              ObjectPtr aRefd;
              if (aRefRes == aReferencedResults.end()) {
                aRefd = aReferencedFeature;
                aCheckFeature = false;
              } else {
                aRefd = *aRefRes;
                if (aRefd->groupName() != ModelAPI_ResultBody::group())
                  break;
              }
              if (!aRefd->data().get() || !aRefd->data()->isValid())
                continue;
              const std::set<AttributePtr>& aRefsToRef = aRefd->data()->refsToMe();
              std::set<AttributePtr>::const_iterator aRR = aRefsToRef.cbegin();
              for(; aRR != aRefsToRef.cend(); aRR++) {
                FeaturePtr aRefFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRR)->owner());
                if (!aRefFeat.get() || aRefFeat == aFeat)
                  continue;
                if (ModelAPI_Session::get()->validators()->isConcealed(
                    aRefFeat->getKind(), (*aRR)->id())) {
                  // check this feature is later than another referenced to make unit tests working
                  //because of Test1757 and others: allow to move selection context of this to next
                  if (aFeat->document()->isLater(aFeat, aRefFeat)) {
                    theError = "Reference to concealed object %1";
                    theError.arg(aRefd->data()->name());
                    return false;
                  }
                }
              }
              if (aCheckFeature)
                aRefRes++;
            }
          }
        }
      }
    }
  }
  return true;
}
