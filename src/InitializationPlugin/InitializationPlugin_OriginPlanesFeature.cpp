// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * InitializationPlugin_OriginPlanesFeature.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <InitializationPlugin_OriginPlanesFeature.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeDouble.h>

InitializationPlugin_OriginPlanesFeature::InitializationPlugin_OriginPlanesFeature()
: ModelAPI_Feature()
{
}

InitializationPlugin_OriginPlanesFeature::~InitializationPlugin_OriginPlanesFeature()
{
}

/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void InitializationPlugin_OriginPlanesFeature::initAttributes()
{
}

/*
 * Computes or recomputes the results
 */
void InitializationPlugin_OriginPlanesFeature::execute()
{
  std::shared_ptr<ModelAPI_Session> aSession = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aSession->activeDocument();
  aSession->startOperation();
  createPoint(aDoc);

  //std::shared_ptr<ModelAPI_Feature> aPlane = aDoc->addFeature(ConstructionPlugin_Plane::ID());

  aSession->finishOperation();
}

void InitializationPlugin_OriginPlanesFeature
::createPoint(const std::shared_ptr<ModelAPI_Document>& aDoc)
{
  std::shared_ptr<ModelAPI_Feature> aPoint = aDoc->addFeature("Point");
  aPoint->real("x")->setValue(0.);
  aPoint->real("y")->setValue(0.);
  aPoint->real("z")->setValue(0.);
}
