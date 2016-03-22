// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "FeaturesPlugin_ValidatorExtrusionBase.h"

#include "GeomValidators_FeatureKind.h"
#include "GeomValidators_ShapeType.h"

#include "ModelAPI_AttributeSelectionList.h"
#include "ModelAPI_ResultPart.h"
#include "ModelAPI_ResultBody.h"
#include "ModelAPI_ResultCompSolid.h"
#include "ModelAPI_Session.h"

bool FeaturesPlugin_ValidatorExtrusionBase::isValid(const AttributePtr& theAttribute,
                                                const std::list<std::string>& theArguments,
                                                std::string& theError) const
{
  bool aValid = true;

  GeomValidators_FeatureKind* aValidator = new GeomValidators_FeatureKind();
  // check whether the selection is on the sketch
  bool aFeatureKindValid = aValidator->isValid(theAttribute, theArguments, theError);
  if (!aFeatureKindValid) {
    // check if selection has Face selected
    GeomValidators_ShapeType* aShapeType = new GeomValidators_ShapeType();
    std::list<std::string> anArguments;
    anArguments.push_back("face");
    aValid = aShapeType->isValid(theAttribute, anArguments, theError);
  }
  return aValid;
}
