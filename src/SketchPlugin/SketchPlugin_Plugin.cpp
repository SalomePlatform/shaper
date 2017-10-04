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

#include <SketchPlugin_Plugin.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Projection.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_Fillet.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_MacroArc.h>
#include <SketchPlugin_MacroCircle.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_Trim.h>
#include <SketchPlugin_Split.h>
#include <SketchPlugin_Validators.h>
#include <SketchPlugin_ExternalValidator.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_MacroEllipse.h>

#include <SketcherPrs_Tools.h>

#include <Events_Loop.h>
#include <GeomDataAPI_Dir.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Data.h>

#include <Config_PropManager.h>

#include <memory>
#include <sstream>

#ifdef _DEBUG
#include <iostream>
#endif

#define SKETCH_ENTITY_COLOR "225,0,0"
#define SKETCH_EXTERNAL_COLOR "170,0,225"
#define SKETCH_AUXILIARY_COLOR "0,85,0"
#define SKETCH_OVERCONSTRAINT_COLOR "0,0,0"
#define SKETCH_FULLY_CONSTRAINED_COLOR "0,150,0"

//#define SET_PLANES_COLOR_IN_PREFERENCES

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
  aFactory->registerValidator("SketchPlugin_SplitValidator",
                              new SketchPlugin_SplitValidator);
  aFactory->registerValidator("SketchPlugin_TrimValidator",
                              new SketchPlugin_TrimValidator);
  aFactory->registerValidator("SketchPlugin_MiddlePointAttr",
                              new SketchPlugin_MiddlePointAttrValidator);
  aFactory->registerValidator("SketchPlugin_ArcTangentPoint",
                              new SketchPlugin_ArcTangentPointValidator);
  aFactory->registerValidator("SketchPlugin_IntersectionValidator",
                              new SketchPlugin_IntersectionValidator);
  aFactory->registerValidator("SketchPlugin_ProjectionValidator",
                              new SketchPlugin_ProjectionValidator);
  aFactory->registerValidator("SketchPlugin_DifferentReference",
                              new SketchPlugin_DifferentReferenceValidator);
  aFactory->registerValidator("SketchPlugin_DifferentPointReference",
                              new SketchPlugin_DifferentPointReferenceValidator);
  aFactory->registerValidator("SketchPlugin_CirclePassedPointValidator",
                              new SketchPlugin_CirclePassedPointValidator);
  aFactory->registerValidator("SketchPlugin_ThirdPointValidator",
                              new SketchPlugin_ThirdPointValidator);
  aFactory->registerValidator("SketchPlugin_ArcEndPointValidator",
                              new SketchPlugin_ArcEndPointValidator);
  aFactory->registerValidator("SketchPlugin_ArcEndPointIntersectionValidator",
                              new SketchPlugin_ArcEndPointIntersectionValidator);
  aFactory->registerValidator("SketchPlugin_HasNoConstraint", new SketchPlugin_HasNoConstraint);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);

  Config_PropManager::registerProp("Visualization", "sketch_entity_color",
                                   "Sketch entity color",
                                   Config_Prop::Color, SKETCH_ENTITY_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_external_color",
                                   "Sketch external entity color",
                                   Config_Prop::Color, SKETCH_EXTERNAL_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_auxiliary_color",
                                   "Sketch auxiliary entity color",
                                   Config_Prop::Color, SKETCH_AUXILIARY_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_dimension_color",
                                   "Sketch dimension color",
                                   Config_Prop::Color, SKETCH_DIMENSION_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_overconstraint_color",
                                   "Sketch overconstraint color",
                                   Config_Prop::Color, SKETCH_OVERCONSTRAINT_COLOR);

  Config_PropManager::registerProp("Visualization", "sketch_fully_constrained_color",
                                   "Sketch fully constrained color",
                                   Config_Prop::Color, SKETCH_FULLY_CONSTRAINED_COLOR);

  // register sketcher properties
#ifdef SET_PLANES_COLOR_IN_PREFERENCES
  Config_PropManager::registerProp("Visualization", "yz_plane_color", "YZ plane color",
                                   Config_Prop::Color, YZ_PLANE_COLOR);
  Config_PropManager::registerProp("Visualization", "xz_plane_color", "XZ plane color",
                                   Config_Prop::Color, XZ_PLANE_COLOR);
  Config_PropManager::registerProp("Visualization", "xy_plane_color", "XY plane color",
                                   Config_Prop::Color, XY_PLANE_COLOR);
#endif

  Config_PropManager::registerProp(SKETCH_TAB_NAME, "dimension_font", "Dimension font",
    Config_Prop::String, "Times-bold");
  std::ostringstream aStream;
  aStream << SketcherPrs_Tools::getDefaultTextHeight();

  Config_PropManager::registerProp(SKETCH_TAB_NAME, "dimension_value_size",
    "Dimension value size", Config_Prop::Integer, aStream.str());
  aStream.str("");
  aStream.clear();
  aStream << SketcherPrs_Tools::getDefaultArrowSize();
  Config_PropManager::registerProp(SKETCH_TAB_NAME, "dimension_arrow_size",
    "Dimension arrow size", Config_Prop::Integer, aStream.str());
}

FeaturePtr SketchPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == SketchPlugin_Sketch::ID()) {
    return FeaturePtr(new SketchPlugin_Sketch);
  } else if (theFeatureID == SketchPlugin_Point::ID()) {
    return FeaturePtr(new SketchPlugin_Point);
  } else if (theFeatureID == SketchPlugin_IntersectionPoint::ID()) {
    return FeaturePtr(new SketchPlugin_IntersectionPoint);
  } else if (theFeatureID == SketchPlugin_Line::ID()) {
    return FeaturePtr(new SketchPlugin_Line);
  } else if (theFeatureID == SketchPlugin_Circle::ID()) {
    return FeaturePtr(new SketchPlugin_Circle);
  } else if (theFeatureID == SketchPlugin_Arc::ID()) {
    return FeaturePtr(new SketchPlugin_Arc);
  } else if (theFeatureID == SketchPlugin_Projection::ID()) {
    return FeaturePtr(new SketchPlugin_Projection);
  } else if (theFeatureID == SketchPlugin_ConstraintCoincidence::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintCoincidence);
  } else if (theFeatureID == SketchPlugin_ConstraintCollinear::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintCollinear);
  } else if (theFeatureID == SketchPlugin_ConstraintDistance::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintDistance);
  } else if (theFeatureID == SketchPlugin_ConstraintDistanceHorizontal::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintDistanceHorizontal);
  } else if (theFeatureID == SketchPlugin_ConstraintDistanceVertical::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintDistanceVertical);
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
  } else if (theFeatureID == SketchPlugin_ConstraintMiddle::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintMiddle);
  } else if (theFeatureID == SketchPlugin_ConstraintMirror::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintMirror);
  } else if (theFeatureID == SketchPlugin_Fillet::ID()) {
    return FeaturePtr(new SketchPlugin_Fillet);
  } else if (theFeatureID == SketchPlugin_MultiTranslation::ID()) {
    return FeaturePtr(new SketchPlugin_MultiTranslation);
  } else if (theFeatureID == SketchPlugin_MultiRotation::ID()) {
    return FeaturePtr(new SketchPlugin_MultiRotation);
  } else if (theFeatureID == SketchPlugin_ConstraintAngle::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintAngle);
  } else if (theFeatureID == SketchPlugin_Split::ID()) {
    return FeaturePtr(new SketchPlugin_Split);
  } else if (theFeatureID == SketchPlugin_Trim::ID()) {
    return FeaturePtr(new SketchPlugin_Trim);
  } else if (theFeatureID == SketchPlugin_MacroArc::ID()) {
    return FeaturePtr(new SketchPlugin_MacroArc);
  } else if (theFeatureID == SketchPlugin_MacroCircle::ID()) {
    return FeaturePtr(new SketchPlugin_MacroCircle);
  } else if (theFeatureID == SketchPlugin_Ellipse::ID()) {
    return FeaturePtr(new SketchPlugin_Ellipse);
  } else if (theFeatureID == SketchPlugin_MacroEllipse::ID()) {
    return FeaturePtr(new SketchPlugin_MacroEllipse);
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
        std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
      // it is important to check whether the normal attribute is initialized
      // because it is possible that normal values are filled when the plane is checked on validity
      aHasSketchPlane = aNormal && aNormal->isInitialized() &&
                        !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);

      aMsg->setState(SketchPlugin_Point::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_IntersectionPoint::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Line::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Circle::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Arc::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Ellipse::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Projection::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintCoincidence::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintCollinear::ID(), aHasSketchPlane);
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
      aMsg->setState(SketchPlugin_ConstraintMiddle::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintMirror::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Fillet::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintAngle::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_MultiRotation::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_MultiTranslation::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Split::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_Trim::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_MacroArc::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_MacroCircle::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintDistanceHorizontal::ID(), aHasSketchPlane);
      aMsg->setState(SketchPlugin_ConstraintDistanceVertical::ID(), aHasSketchPlane);
      // SketchRectangle is a python feature, so its ID is passed just as a string
      aMsg->setState("SketchRectangle", aHasSketchPlane);
    }
  }
  return aMsg;
}
