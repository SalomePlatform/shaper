// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionPlugin_Field.cpp
// Created:     08 Oct 2014
// Author:      Sergey BELASH

#include "CollectionPlugin_Field.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeTables.h>

CollectionPlugin_Field::CollectionPlugin_Field()
{
}

void CollectionPlugin_Field::initAttributes()
{
  data()->addAttribute(SELECTED_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(COMPONENTS_NB_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(COMPONENTS_NAMES_ID(), ModelAPI_AttributeStringArray::typeId());
  data()->addAttribute(VALUES_TYPE_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(STEPS_NB_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(STAMPS_ID(), ModelAPI_AttributeIntArray::typeId());
  data()->addAttribute(VALUES_ID(), ModelAPI_AttributeTables::typeId());
}

void CollectionPlugin_Field::execute()
{
  if (results().empty() || firstResult()->isDisabled()) { // just create result if not exists
    //ResultPtr aField = document()->createField(data());
    //setResult(aField);
  }
}
