// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_IconFactory.cpp
// Created:     28 Jul 2015
// Author:      Vitaly SMETANNIKOV

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
        anIcon = QIcon(anIconString);
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
    if (aShape.get()) {
      if (aShape->isSolid()) 
        return QIcon(":pictures/solid.png");
      else if (aShape->isCompound()) 
        return QIcon(":pictures/compound.png");
      else if (aShape->isCompoundOfSolids()) 
        return QIcon(":pictures/compoundofsolids.png");
      else if (aShape->isCompSolid()) 
        return QIcon(":pictures/compsolid.png");
      else if (aShape->isEdge()) 
        return QIcon(":pictures/edge.png");
      else if (aShape->isFace()) 
        return QIcon(":pictures/face.png");
      else if (aShape->isVertex()) 
        return QIcon(":pictures/vertex.png");
    }
  }
  return anIcon;  
}

void PartSet_IconFactory::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(Config_FeatureMessage::GUI_EVENT())) {
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