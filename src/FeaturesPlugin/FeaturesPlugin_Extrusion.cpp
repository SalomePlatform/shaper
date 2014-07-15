// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Extrusion.h"
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeBoolean.h>

#include <GeomAlgoAPI_Extrusion.h>

using namespace std;

FeaturesPlugin_Extrusion::FeaturesPlugin_Extrusion()
{
}

void FeaturesPlugin_Extrusion::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Extrusion::FACE_ID(), ModelAPI_AttributeReference::type());
  data()->addAttribute(FeaturesPlugin_Extrusion::SIZE_ID(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(FeaturesPlugin_Extrusion::REVERSE_ID(), ModelAPI_AttributeBoolean::type());
}

void FeaturesPlugin_Extrusion::execute()
{
  boost::shared_ptr<ModelAPI_AttributeReference> aFaceRef = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(data()->attribute(FeaturesPlugin_Extrusion::FACE_ID()));
  if (!aFaceRef)
    return;
  FeaturePtr aFaceFeature = aFaceRef->value();
  if (!aFaceFeature)
    return;
  boost::shared_ptr<GeomAPI_Shape> aFace = aFaceFeature->data()->shape();
  if (!aFace)
    return;

  double aSize = data()->real(FeaturesPlugin_Extrusion::SIZE_ID())->value();
  if (data()->boolean(FeaturesPlugin_Extrusion::REVERSE_ID())->value())
    aSize = -aSize;
  data()->store(GeomAlgoAPI_Extrusion::makeExtrusion(aFace, aSize));
}
