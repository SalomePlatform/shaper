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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Model_ResultBody.h>
#include <Model_BodyBuilder.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>
#include <Config_PropManager.h>
#include <ModelAPI_Events.h>
// DEB
//#include <TCollection_AsciiString.hxx>
//#include <TDF_Tool.hxx>
//#define DEB_IMPORT 1

Model_ResultBody::Model_ResultBody()
{
  myBuilder = new Model_BodyBuilder(this);
  myWasConcealed = false;
  myConnect = ConnectionNotComputed;
}

void Model_ResultBody::colorConfigInfo(std::string& theSection, std::string& theName,
  std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_body_color";
  theDefault = DEFAULT_COLOR();
}

bool Model_ResultBody::setDisabled(std::shared_ptr<ModelAPI_Result> theThis, const bool theFlag)
{
  bool aChanged = ModelAPI_ResultBody::setDisabled(theThis, theFlag);
  if (aChanged) { // state is changed, so modifications are needed
    myBuilder->evolutionToSelection(theFlag);
  }
  return aChanged;
}

bool Model_ResultBody::isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  return myBuilder->isLatestEqual(theShape);
}

bool Model_ResultBody::isConcealed()
{
  bool aResult = false;
  if (ModelAPI_ResultBody::isConcealed()) {
    aResult = true;
  } else {
    ResultPtr aThis = std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner());
    if (aThis.get()) {
      ResultCompSolidPtr aParent = ModelAPI_Tools::compSolidOwner(aThis);
      if (aParent.get()) {
        if (aParent->isConcealed())
          aResult = true;
      }
    }
  }
  if (myWasConcealed != aResult) {
    myWasConcealed = aResult;
    if (aResult) { // hidden unit must be redisplayed (hidden)
      ModelAPI_EventCreator::get()->sendDeleted(document(), this->groupName());
      // redisplay for the viewer (it must be disappeared also)
      static Events_ID EVENT_DISP =
        Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), EVENT_DISP);
    } else { // was not concealed become concealed => delete event
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
      ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), anEvent);
    }
  }

  return aResult;
}
