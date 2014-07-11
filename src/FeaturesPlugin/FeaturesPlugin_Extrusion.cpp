// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Extrusion.h"
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
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
  data()->addAttribute(EXTRUSION_FACE, ModelAPI_AttributeReference::type());
  data()->addAttribute(EXTRUSION_SIZE, ModelAPI_AttributeDouble::type());
  data()->addAttribute(EXTRUSION_REVERSE, ModelAPI_AttributeBoolean::type());
}

void FeaturesPlugin_Extrusion::execute()
{
  boost::shared_ptr<ModelAPI_AttributeReference> aFaceRef = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(data()->attribute(EXTRUSION_FACE));
  if (!aFaceRef)
    return;
  FeaturePtr aFaceFeature = aFaceRef->value();
  if (!aFaceFeature)
    return;
  boost::shared_ptr<ModelAPI_ResultConstruction> aConstr =
    boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aFaceFeature->firstResult());
  if (!aConstr) 
    return;
  boost::shared_ptr<GeomAPI_Shape> aFace = aConstr->shape();
  if (!aFace)
    return;

  double aSize = data()->real(EXTRUSION_SIZE)->value();
  if (data()->boolean(EXTRUSION_REVERSE)->value())
    aSize = -aSize;
  boost::shared_ptr<ModelAPI_ResultBody> aResult = document()->createBody();
  aResult->store(GeomAlgoAPI_Extrusion::makeExtrusion(aFace, aSize));
  document()->storeResult(data(), aResult);
  setResult(aResult);
}
