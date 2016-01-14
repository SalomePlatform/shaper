// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <SketchPlugin_Plugin.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_Validators.h>
#include <SketchPlugin_ExternalValidator.h>

#include <Events_Loop.h>
#include <GeomDataAPI_Dir.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Data.h>

#include <Config_PropManager.h>

#include <memory>

#ifdef _DEBUG
#include <iostream>
#endif

//#define SET_PLANES_COLOR_IN_PREFERENCES

using namespace std;

// the only created instance of this plugin
static SketchPlugin_Plugin* MY_SKETCH_INSTANCE = new SketchPlugin_Plugin();

SketchPlugin_Plugin::SketchPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("SketchPlugin_DistanceAttr",
                              new SketchPlugin_DistanceAttrValidator);  
  aFactory->registerValidator("SketchPlugin_ExternalValidator",
                              new SketchPlugin_ExternalValidator);
  aFactory->registerValidator("SketchPlugin_TangentAttr",
                              new SketchPlugin_TangentAttrValidator);
  aFactory->registerValidator("SketchPlugin_NotFixed",
                              new SketchPlugin_NotFixedValidator);
  aFactory->registerValidator("SketchPlugin_EqualAttr",
                              new SketchPlugin_EqualAttrValidator);
  aFactory->registerValidator("SketchPlugin_MirrorAttr",
                              new SketchPlugin_MirrorAttrValidator);
  aFactory->registerValidator("SketchPlugin_CoincidenceAttr",
                              new SketchPlugin_CoincidenceAttrValidator);
  aFactory->registerValidator("SketchPlugin_CopyValidator",
                              new SketchPlugin_CopyValidator);
  aFactory->registerValidator("SketchPlugin_SolverErrorValidator",
                              new SketchPlugin_SolverErrorValidator);
  aFactory->registerValidator("SketchPlugin_FilletVertexValidator",
                              new SketchPlugin_FilletVertexValidator);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
  
  Config_PropManager::registerProp("Visualization", "sketch_entity_color", "Sketch entity color",
                                   Config_Prop::Color, SKETCH_ENTITY_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_external_color", "Sketch external entity color",
                                   Config_Prop::Color, SKETCH_EXTERNAL_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_auxiliary_color", "Sketch auxiliary entity color",
                                   Config_Prop::Color, SKETCH_AUXILIARY_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_dimension_color", "Sketch dimension color",
                                   Config_Prop::Color, SKETCH_DIMENSION_COLOR);

  // register sketcher properties
#ifdef SET_PLANES_COLOR_IN_PREFERENCES
  Config_PropManager::registerProp("Visualization", "yz_plane_color", "YZ plane color",
                                   Config_Prop::Color, YZ_PLANE_COLOR);
  Config_PropManager::registerProp("Visualization", "xz_plane_color", "XZ plane color",
                                   Config_Prop::Color, XZ_PLANE_COLOR);
  Config_PropManager::registerProp("Visualization", "xy_plane_color", "XY plane color",
                                   Config_Prop::Color, XY_PLANE_COLOR);
#endif
}

FeaturePtr SketchPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == SketchPlugin_Sketch::ID()) {
    return FeaturePtr(new SketchPlugin_Sketch);
  } else if (theFeatureID == SketchPlugin_Point::ID()) {
    return FeaturePtr(new SketchPlugin_Point);
  } else if (theFeatureID == SketchPlugin_Line::ID()) {
    return FeaturePtr(new SketchPlugin_Line);
  } else if (theFeatureID == SketchPlugin_Circle::ID()) {
    return FeaturePtr(new SketchPlugin_Circle);
  } else if (theFeatureID == SketchPlugin_Arc::ID()) {
    return FeaturePtr(new SketchPlugin_Arc);
  } else if (theFeatureID == SketchPlugin_ConstraintCoincidence::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintCoincidence);
  } else if (theFeatureID == SketchPlugin_ConstraintDistance::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintDistance);
  } else if (theFeatureID == SketchPlugin_ConstraintLength::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintLength);
  } else if (theFeatureID == SketchPlugin_ConstraintParallel::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintParallel);
  } else if (theFeatureID == SketchPlugin_ConstraintPerpendicular::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintPerpendicular);
  } else if (theFeatureID == SketchPlugin_ConstraintRadius::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintRadius);
  } else if (theFeatureID == SketchPlugin_ConstraintRigid::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintRigid);
  } else if (theFeatureID == SketchPlugin_ConstraintHorizontal::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintHorizontal);
  } else if (theFeatureID == SketchPlugin_ConstraintVertical::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintVertical);
  } else if (theFeatureID == SketchPlugin_ConstraintEqual::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintEqual);
  } else if (theFeatureID == SketchPlugin_ConstraintTangent::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintTangent);
  } else if (theFeatureID == SketchPlugin_ConstraintMirror::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintMirror);
  } else if (theFeatureID == SketchPlugin_ConstraintFillet::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintFillet);
  } else if (theFeatureID == SketchPlugin_MultiTranslation::ID()) {
    return FeaturePtr(new SketchPlugin_MultiTranslation);
  } else if (theFeatureID == SketchPlugin_MultiRotation::ID()) {
    return FeaturePtr(new SketchPlugin_MultiRotation);
  } else if (theFeatureID == SketchPlugin_ConstraintAngle::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintAngle);
  }
  // feature of such kind is not found
  return FeaturePtr();
}

void SketchPlugin_Plugin::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  const Events_ID kRequestEvent =
      Events_Loop::loop()->eventByName(EVENT_FEATURE_STATE_REQUEST);
  if (theMessage->eventID() == kRequestEvent) {
    std::shared_ptr<ModelAPI_FeatureStateMessage> aStateMessage =
        std::dynamic_pointer_cast<ModelAPI_FeatureStateMessage>(theMessage);
    Events_Loop::loop()->send(getFeaturesState(aStateMessage->feature()), false);
  }
}

std::shared_ptr<ModelAPI_FeatureStateMessage> SketchPlugin_Plugin
::getFeaturesState(const std::shared_ptr<ModelAPI_Feature>& theFeature) const
{
  const Events_ID kResponseEvent = Events_Loop::loop()->eventByName(EVENT_FEATURE_STATE_RESPONSE);
  std::shared_ptr<ModelAPI_FeatureStateMessage> aMsg(
      new ModelAPI_FeatureStateMessage(kResponseEvent, this));

  bool aHasSketchPlane = false;
  std::shared_ptr<SketchPlugin_Sketch> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Sketch>(theFeature);
  if (aSketchFeature.get()) {
    std::shared_ptr<ModelAPI_Data> aData = aSketchFeature->data();
    if (aData) {
      std::shared_ptr<GeomDataAPI_Dir> aNormal =
        std::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SketchPlugin_Sketch::NORM_ID()));
      // it is important to check whether the normal attribute is initialized
      // because it is possible that normal values are filled when the plane is checked on validity
      aHasSketchPlane = aNormal && aNormal->isInitialized() &&
                        !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);

      aMsg->setState(SketchPlugin_Point::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Line::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Circle::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Arc::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintCoincidence::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintDistance::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintLength::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintParallel::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintPerpendicular::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintRadius::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintRigid::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintHorizontal::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintVertical::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintEqual::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintTangent::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintMirror::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintFillet::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintAngle::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_MultiRotation::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_MultiTranslation::ID(), aHasSketchPlane);
    }
  }
  return aMsg;
}
