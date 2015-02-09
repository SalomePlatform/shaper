// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <SketchPlugin_Plugin.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_Validators.h>
#include <SketchPlugin_ResultValidators.h>

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

using namespace std;

// the only created instance of this plugin
static SketchPlugin_Plugin* MY_SKETCH_INSTANCE = new SketchPlugin_Plugin();

SketchPlugin_Plugin::SketchPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("SketchPlugin_DistanceAttr",
                              new SketchPlugin_DistanceAttrValidator);  
  aFactory->registerValidator("SketchPlugin_DifferentObjects",
                              new SketchPlugin_DifferentObjectsValidator);
  aFactory->registerValidator("SketchPlugin_ResultPoint", new SketchPlugin_ResultPointValidator);
  aFactory->registerValidator("SketchPlugin_ResultLine", new SketchPlugin_ResultLineValidator);
  aFactory->registerValidator("SketchPlugin_ResultArc", new SketchPlugin_ResultArcValidator);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);

  // register sketcher properties

  Config_PropManager::registerProp("Visualization", "parallel_color", "Parallel constraint color",
                                   Config_Prop::Color, PARALLEL_COLOR);
  Config_PropManager::registerProp("Visualization", "perpendicular_color",
                                   "Perpendicular constraint color", Config_Prop::Color,
                                   PERPENDICULAR_COLOR);
  Config_PropManager::registerProp("Visualization", "distance_color", "Distance color",
                                   Config_Prop::Color, DISTANCE_COLOR);
  Config_PropManager::registerProp("Visualization", "length_color", "Length color",
                                   Config_Prop::Color, LENGTH_COLOR);
  Config_PropManager::registerProp("Visualization", "radius_color", "Radius color",
                                   Config_Prop::Color, RADIUS_COLOR);
  Config_PropManager::registerProp("Visualization", "fixing_color", "Fixing color",
                                   Config_Prop::Color, FIXING_COLOR);
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
  std::shared_ptr<ModelAPI_FeatureStateMessage> aMsg =
      std::make_shared<ModelAPI_FeatureStateMessage>(kResponseEvent, this);

  bool aHasSketchPlane = false;
  std::shared_ptr<SketchPlugin_Sketch> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Sketch>(theFeature);
  if (aSketchFeature.get()) {
    std::shared_ptr<ModelAPI_Data> aData = aSketchFeature->data();
    if (aData) {
      std::shared_ptr<GeomDataAPI_Dir> aNormal =
        std::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SketchPlugin_Sketch::NORM_ID()));
      aHasSketchPlane = aNormal && !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);

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
    }
  }
  return aMsg;
}
