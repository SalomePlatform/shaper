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

using namespace std;

ConstructionPlugin_Extrusion::ConstructionPlugin_Extrusion()
{
}

void ConstructionPlugin_Extrusion::initAttributes()
{
  data()->addAttribute(EXTRUSION_FACE, ModelAPI_AttributeReference::type());
  data()->addAttribute(EXTRUSION_SIZE, ModelAPI_AttributeDouble::type());
  data()->addAttribute(EXTRUSION_REVERCE, ModelAPI_AttributeBoolean::type());
}

// this is for debug only
#include <iostream>
void ConstructionPlugin_Extrusion::execute() 
{
  // TODO: create a real shape for the point using OCC layer
  //cout<<"X="<<data()->real(POINT_ATTR_X)->value()<<" Y="<<data()->real(POINT_ATTR_Y)->value()
  //    <<" Z="<<data()->real(POINT_ATTR_Z)->value()<<endl;
}
