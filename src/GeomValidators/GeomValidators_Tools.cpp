// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Tools.cpp
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#include "GeomValidators_Tools.h"

#include "ModelAPI_AttributeRefAttr.h"
#include "ModelAPI_AttributeSelection.h"
#include "ModelAPI_AttributeReference.h"

#include <TopoDS_Iterator.hxx>

namespace GeomValidators_Tools {

  ObjectPtr getObject(const AttributePtr& theAttribute)
  {
    ObjectPtr anObject;
    std::string anAttrType = theAttribute->attributeType();
    if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
      AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
      if (anAttr != NULL && anAttr->isObject())
        anObject = anAttr->object();
    }
    if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
      AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
      if (anAttr != NULL)
        anObject = anAttr->context();
    }
    if (anAttrType == ModelAPI_AttributeReference::typeId()) {
      AttributeReferencePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
      if (anAttr.get() != NULL)
        anObject = anAttr->value();
    }
    return anObject;
  }

  TopAbs_ShapeEnum getCompoundSubType(const TopoDS_Shape& theShape)
  {
    TopAbs_ShapeEnum aShapeType = theShape.ShapeType();

    // for compounds check sub-shapes: it may be compound of needed type:
    // Booleans may produce compounds of Solids
    if (aShapeType == TopAbs_COMPOUND) {
      for(TopoDS_Iterator aSubs(theShape); aSubs.More(); aSubs.Next()) {
        if (!aSubs.Value().IsNull()) {
          TopAbs_ShapeEnum aSubType = aSubs.Value().ShapeType();
          if (aSubType == TopAbs_COMPOUND) { // compound of compound(s)
            aShapeType = TopAbs_COMPOUND;
            break;
          }
          if (aShapeType == TopAbs_COMPOUND) {
            aShapeType = aSubType;
          } else if (aShapeType != aSubType) { // compound of shapes of different types
            aShapeType = TopAbs_COMPOUND;
            break;
          }
        }
      }
    }
    return aShapeType;
  }

}
