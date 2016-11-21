// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionPlugin_Group.cpp
// Created:     08 Oct 2014
// Author:      Sergey BELASH

#include "CollectionPlugin_Group.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultGroup.h>

CollectionPlugin_Group::CollectionPlugin_Group()
{
}

void CollectionPlugin_Group::initAttributes()
{
  data()->addAttribute(CollectionPlugin_Group::LIST_ID(), 
                       ModelAPI_AttributeSelectionList::typeId());
}

void CollectionPlugin_Group::execute()
{
  if (results().empty() || firstResult()->isDisabled()) { // just create result if not exists
    ResultPtr aGroup = document()->createGroup(data());
    setResult(aGroup);
  }
}
