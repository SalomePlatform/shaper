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

#include "PartSet_IconFactory.h"
#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>

#include <Config_FeatureMessage.h>
#include <Events_Loop.h>

QMap<QString, QString> PartSet_IconFactory::myIcons;

PartSet_IconFactory::PartSet_IconFactory():ModuleBase_IconFactory()
{
  Events_Loop::loop()->registerListener(this,
    Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
}


QIcon PartSet_IconFactory::getIcon(ObjectPtr theObj)
{
  QIcon anIcon;

  if (!theObj.get())
    return anIcon;

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature.get()) {
    std::string aKind = aFeature->getKind();
    QString aId(aKind.c_str());
    if (!myIcons.contains(aId))
      return anIcon;

    QString anIconString = myIcons[aId];

    ModelAPI_ExecState aState = aFeature->data()->execState();
    switch(aState) {
      case ModelAPI_StateDone:
      case ModelAPI_StateNothing: {
        anIcon = loadIcon(anIconString);
      }
      break;
      case ModelAPI_StateMustBeUpdated: {
        anIcon = ModuleBase_Tools::lighter(anIconString);
      }
      break;
      case ModelAPI_StateExecFailed: {
        anIcon = ModuleBase_Tools::composite(":icons/exec_state_failed.png", anIconString);
      }
      break;
      case ModelAPI_StateInvalidArgument: {
        anIcon = ModuleBase_Tools::composite(":icons/exec_state_invalid_parameters.png",
                                             anIconString);
      }
      break;
      default: break;
    }
  }

  if (theObj->data() && theObj->data()->execState() == ModelAPI_StateMustBeUpdated)
    return QIcon(":pictures/constr_object_modified.png");

  std::string aGroup = theObj->groupName();
  if (aGroup == ModelAPI_ResultPart::group())
    return QIcon(":pictures/part_ico.png");

  if (aGroup == ModelAPI_ResultConstruction::group())
    return QIcon(":pictures/constr_object.png");

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObj);
  if (aResult.get()) {
    GeomShapePtr aShape = aResult->shape();
    if(aShape.get()) {
      switch(aShape->shapeType()) {
        case GeomAPI_Shape::COMPOUND: {
          ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
          if (aBody.get() && aBody->isConnectedTopology())
            return QIcon(":pictures/compoundofsolids.png");
          return QIcon(":pictures/compound.png");
        }
        case GeomAPI_Shape::COMPSOLID: return QIcon(":pictures/compsolid.png");
        case GeomAPI_Shape::SOLID:     return QIcon(":pictures/solid.png");
        case GeomAPI_Shape::SHELL:     return QIcon(":pictures/shell.png");
        case GeomAPI_Shape::FACE:      return QIcon(":pictures/face.png");
        case GeomAPI_Shape::WIRE:      return QIcon(":pictures/wire.png");
        case GeomAPI_Shape::EDGE:      return QIcon(":pictures/edge.png");
        case GeomAPI_Shape::VERTEX:    return QIcon(":pictures/vertex.png");
      }
    }
  }
  return anIcon;
}

void PartSet_IconFactory::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() ==
      Events_Loop::loop()->eventByName(Config_FeatureMessage::GUI_EVENT())) {
    std::shared_ptr<Config_FeatureMessage> aFeatureMsg =
       std::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (!aFeatureMsg->isInternal()) {
      ActionInfo aFeatureInfo;
      aFeatureInfo.initFrom(aFeatureMsg);
      // Remember features icons
      myIcons[QString::fromStdString(aFeatureMsg->id())] = aFeatureInfo.iconFile;
    }
  }
}