// File:        PartSetPlugin_Point.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Point.h"
#include "ModelAPI_PluginManager.h"
#include "ModelAPI_Document.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_AttributeDouble.h"

using namespace std;

PartSetPlugin_Point::PartSetPlugin_Point()
{
}

void PartSetPlugin_Point::initAttributes()
{
  data()->addAttribute(POINT_ATTR_X, ModelAPI_AttributeDouble::type());
  data()->addAttribute(POINT_ATTR_Y, ModelAPI_AttributeDouble::type());
  data()->addAttribute(POINT_ATTR_Z, ModelAPI_AttributeDouble::type());
}

// this is for debug only
#include <iostream>
void PartSetPlugin_Point::execute() 
{
  // TODO: create a real shape for the point using OCC layer
  cout<<"X="<<data()->real(POINT_ATTR_X)->value()<<" Y="<<data()->real(POINT_ATTR_Y)->value()
      <<" Z="<<data()->real(POINT_ATTR_Z)->value()<<endl;
}
