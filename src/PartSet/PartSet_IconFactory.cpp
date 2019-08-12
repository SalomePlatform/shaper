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

#include "PartSet_IconFactory.h"
#include "PartSet_Tools.h"
#include "PartSet_Module.h"
#include "XGUI_DataModel.h"

#include <XGUI_Workshop.h>
#include <XGUI_ObjectsBrowser.h>

#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>

#include <Config_FeatureMessage.h>
#include <Events_Loop.h>

QMap<QString, QString> PartSet_IconFactory::myIcons;

PartSet_IconFactory::PartSet_IconFactory(PartSet_Module* theModule)
  : ModuleBase_IconFactory(), myModule(theModule)
{
  Events_Loop::loop()->registerListener(this,
    Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
}


QIcon PartSet_IconFactory::getIcon(ObjectPtr theObj)
{
  if (!theObj.get())
    return QIcon();

  std::string aGroup = theObj->groupName();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature.get()) {
    std::string aKind = aFeature->getKind();
    QString aId(aKind.c_str());
    if (!myIcons.contains(aId))
      return QIcon();

    QString anIconString = myIcons[aId];

    ModelAPI_ExecState aState = aFeature->data()->execState();
    switch (aState) {
    case ModelAPI_StateMustBeUpdated:
      return ModuleBase_Tools::composite(":icons/toWork.png", anIconString);
      //anIcon = ModuleBase_Tools::lighter(anIconString);
      break;
    case ModelAPI_StateExecFailed:
      return ModuleBase_Tools::composite(":icons/isFailed.png", anIconString);
      break;
    case ModelAPI_StateInvalidArgument:
      return ModuleBase_Tools::composite(":icons/exec_state_invalid_parameters.png",
        anIconString);
      break;
    default:
      return loadIcon(anIconString);
      break;
    }
  }

  //if (theObj->data() && theObj->data()->execState() == ModelAPI_StateMustBeUpdated)
  //  return QIcon(":pictures/constr_object_modified.png");

  if (aGroup == ModelAPI_ResultPart::group())
    return QIcon(":pictures/part_ico.png");

  if (aGroup == ModelAPI_ResultConstruction::group())
    return QIcon(":pictures/constr_object.png");

  if (aGroup == ModelAPI_Folder::group()) {
    static QString anIconString(":pictures/features_folder.png");
    int aFirst = -1, aLast = -1;
    PartSet_Tools::getFirstAndLastIndexInFolder(theObj, aFirst, aLast);
    if ((aFirst != -1) && (aLast != -1)) {
      int aNbItems = aLast - aFirst + 1;
      if (aNbItems) {
        XGUI_ObjectsBrowser* aObBrowser = myModule->getWorkshop()->objectBrowser();
        XGUI_DataTree* aTree = aObBrowser->treeView();
        QModelIndex aIndex = aTree->dataModel()->objectIndex(theObj, 0);
        if (!aTree->isExpanded(aIndex)) {
          DocumentPtr aDoc = theObj->document();
          ObjectPtr aSubObj;
          ModelAPI_ExecState aState;
          bool aHasWarning = false;
          for (int i = aFirst; i < aLast + 1; i++) {
            aSubObj = aDoc->object(ModelAPI_Feature::group(), i);
            aState = aSubObj->data()->execState();
            if ((aState == ModelAPI_StateExecFailed) || (aState == ModelAPI_StateMustBeUpdated)) {
              aHasWarning = true;
              break;
            }
          }
          if (aHasWarning) {
            return QIcon(ModuleBase_Tools::composite(":icons/hasWarning.png", anIconString));
          }
        }
      }
    }
    return loadIcon(anIconString);
  }

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObj);
  if (aResult.get()) {
    GeomShapePtr aShape = aResult->shape();
    if(aShape.get()) {
      switch(aShape->shapeType()) {
        case GeomAPI_Shape::COMPOUND: {
          if (aResult->groupName() == ModelAPI_ResultGroup::group()) {
            switch (aShape->typeOfCompoundShapes()) {
            case GeomAPI_Shape::VERTEX:
              return QIcon(":icons/group_vertex.png");
            case GeomAPI_Shape::EDGE:
              return QIcon(":icons/group_edge.png");
            case GeomAPI_Shape::FACE:
              return QIcon(":icons/group_face.png");
            case GeomAPI_Shape::SOLID:
              return QIcon(":icons/group_solid.png");
            }
          }
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
  return QIcon();
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