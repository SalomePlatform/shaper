// File:        FeaturesPlugin_Group.cpp
// Created:     08 Oct 2014
// Author:      Sergey BELASH

#include "FeaturesPlugin_Group.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultGroup.h>


using namespace std;

FeaturesPlugin_Group::FeaturesPlugin_Group()
{
}

void FeaturesPlugin_Group::initAttributes()
{
  //data()->addAttribute(FeaturesPlugin_Group::NAME_ID(), ModelAPI_AttributeString::type());
  data()->addAttribute(FeaturesPlugin_Group::LIST_ID(), ModelAPI_AttributeSelectionList::type());
}

void FeaturesPlugin_Group::execute()
{
  if (results().empty()) { // just create result if not exists
    ResultPtr aGroup = document()->createGroup(data());
    setResult(aGroup);
  }
}
