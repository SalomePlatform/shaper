// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Feature.cpp
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#include "SketchShapePlugin_Feature.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

SketchShapePlugin_Feature::SketchShapePlugin_Feature()
: ModelAPI_Feature()
{
}

void SketchShapePlugin_Feature::initAttributes()
{
  data()->addAttribute(SKETCH_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(VERTEX_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(VERTEX_CHOICE_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VERTEX_CHOICE_ID());

  data()->addAttribute(EDGE_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(EDGE_CHOICE_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EDGE_CHOICE_ID());

  data()->addAttribute(FACE_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(FACE_CHOICE_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FACE_CHOICE_ID());
}

void SketchShapePlugin_Feature::execute()
{
}

void SketchShapePlugin_Feature::attributeChanged(const std::string& theID)
{
  if (theID == VERTEX_CHOICE_ID() ||
      theID == EDGE_CHOICE_ID() ||
      theID == FACE_CHOICE_ID()) {
    std::string aListAttrId = theID == VERTEX_CHOICE_ID() ? VERTEX_LIST_ID() : (
                              theID == EDGE_CHOICE_ID() ? EDGE_LIST_ID() :
                              FACE_LIST_ID());

    AttributeBooleanPtr aChoiceAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
                                           data()->attribute(theID));
    if (!aChoiceAttribute->value()) {
      AttributeSelectionListPtr aListAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->attribute(aListAttrId));
      aListAttribute->clear();
    }
  }
  else if (theID == VERTEX_LIST_ID() ||
           theID == EDGE_LIST_ID() ||
           theID == FACE_LIST_ID()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->attribute(theID));
    for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      ObjectPtr anObject = aSelectAttr->context();
      if (!anObject.get())
        continue;
      else {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      }
    }
  }
}

