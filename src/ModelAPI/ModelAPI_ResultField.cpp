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

#include "ModelAPI_ResultField.h"
#include "ModelAPI_Events.h"
#include <Events_Loop.h>

ModelAPI_ResultField::~ModelAPI_ResultField()
{

}

std::string ModelAPI_ResultField::groupName()
{
  return group();
}

void ModelAPI_ResultField::ModelAPI_FieldStep::setDisplayed(const bool theDisplay)
{
  if (myIsDisplayed != theDisplay) {
    myIsDisplayed = theDisplay;
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
    ModelAPI_ResultField* aField = field();
    if (aField) {
      aECreator->sendUpdated(aField->step(id()), EVENT_DISP); // updated pointer to this
      if (myIsDisplayed) {
        for (int i = 0; i < aField->stepsSize(); i++)
          if (i != id())
            aField->step(i)->setDisplayed(false);
        aField->setDisplayed(false);
      }
    }
  }
}


void ModelAPI_ResultField::setDisplayed(const bool theDisplay)
{
  ModelAPI_Result::setDisplayed(theDisplay);
  if (isDisplayed()) {
    for (int i = 0; i < stepsSize(); i++)
      step(i)->setDisplayed(false);
  }
}
