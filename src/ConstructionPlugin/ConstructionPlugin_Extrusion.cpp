// File:        ConstructionPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "ConstructionPlugin_Extrusion.h"
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeBoolean.h>

#include <GeomAlgoAPI_Extrusion.h>

using namespace std;

ConstructionPlugin_Extrusion::ConstructionPlugin_Extrusion()
{
}

void ConstructionPlugin_Extrusion::initAttributes()
{
  data()->addAttribute(EXTRUSION_FACE, ModelAPI_AttributeReference::type());
  data()->addAttribute(EXTRUSION_SIZE, ModelAPI_AttributeDouble::type());
  data()->addAttribute(EXTRUSION_REVERSE, ModelAPI_AttributeBoolean::type());
}

void ConstructionPlugin_Extrusion::execute()
{
  boost::shared_ptr<ModelAPI_AttributeReference> aFaceRef = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(data()->attribute(EXTRUSION_FACE));
  if (!aFaceRef)
    return;
  FeaturePtr aFaceFeature = aFaceRef->value();
  if (!aFaceFeature)
    return;
  boost::shared_ptr<GeomAPI_Shape> aFace = aFaceFeature->data()->shape();
  if (!aFace)
    return;

  double aSize = data()->real(EXTRUSION_SIZE)->value();
  if (data()->boolean(EXTRUSION_REVERSE)->value())
    aSize = -aSize;
  data()->store(GeomAlgoAPI_Extrusion::makeExtrusion(aFace, aSize));
}
