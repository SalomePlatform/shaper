// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Validators.cpp
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#include "SketchShapePlugin_Validators.h"

//#include "SketchShapePlugin_Feature.h"

bool SketchShapePlugin_FeatureValidator::isValid(const AttributePtr& theAttribute, 
                                                 const std::list<std::string>& theArguments,
                                                 std::string& theError) const
{
  /*if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }*/

  return true;
}
