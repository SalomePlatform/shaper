// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "GeomValidators_EdgeOrVertex.h"
#include "GeomValidators_Tools.h"
#include "GeomValidators_Edge.h"

#include "ModelAPI_AttributeRefAttr.h"
#include "ModelAPI_Result.h"

#include <ModelAPI_Session.h>

#include <GeomAPI_Curve.h>
#include <GeomDataAPI_Point2D.h>

#include <Events_Error.h>

#include <StdSelect_TypeOfEdge.hxx>

#include <QString>
#include <QMap>


bool GeomValidators_EdgeOrVertex::isValid(const AttributePtr& theAttribute,
                                          const std::list<std::string>& theArguments) const
{
  bool aValid = false;

  // 1. check whether the attribute is a linear edge
    // there is a check whether the feature contains a point and a linear edge or two point values
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  const GeomValidators_Edge* aLinearEdgeValidator =
    dynamic_cast<const GeomValidators_Edge*>(aFactory->validator("GeomValidators_Edge"));

  std::list<std::string> anArguments;
  anArguments.push_back("line");
  aValid = aLinearEdgeValidator->isValid(theAttribute, anArguments);
  if (!aValid) {
    //2. check whether the attribute is a vertex
    ObjectPtr anObject = GeomValidators_Tools::getObject(theAttribute);
    if (anObject.get() != NULL) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      if (aResult.get() != NULL) {
        GeomShapePtr aShape = aResult->shape();
        if (aShape.get() != NULL) {
          aValid = aShape->isVertex();
        }
      }
    }
    else {
      AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
      if (anAttr.get() != NULL) {
        AttributePtr aRefAttr = anAttr->attr();
        aValid = aRefAttr.get() != NULL && aRefAttr->attributeType() == GeomDataAPI_Point2D::typeId();
      }
    }
  }
  return aValid;
}
