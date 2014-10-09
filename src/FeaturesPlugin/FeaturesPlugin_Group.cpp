// File:        FeaturesPlugin_Group.cpp
// Created:     08 Oct 2014
// Author:      Sergey BELASH

#include "FeaturesPlugin_Group.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>

using namespace std;

FeaturesPlugin_Group::FeaturesPlugin_Group()
{
}

void FeaturesPlugin_Group::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Group::NAME_ID(), ModelAPI_AttributeString::type());
  data()->addAttribute(FeaturesPlugin_Group::LIST_ID(), ModelAPI_AttributeString::type());
}

void FeaturesPlugin_Group::execute()
{
  AttributeStringPtr aNameAttr = boost::dynamic_pointer_cast<ModelAPI_AttributeString>(
      data()->attribute(FeaturesPlugin_Group::NAME_ID()));
  if (!aNameAttr)
    return;
  std::string aName = aNameAttr->value();
  data()->setName(aName);
}
