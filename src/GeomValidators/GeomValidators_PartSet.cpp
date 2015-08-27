// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_PartSet.cpp
// Created:     27 Aug 2015
// Author:      Natalia ERMOLAEVA

#include <GeomValidators_PartSet.h>
#include <GeomValidators_Tools.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>

#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>

bool GeomValidators_PartSet::isValid(const AttributePtr& theAttribute,
                                   const std::list<std::string>& theArguments,
                                   std::string& theError) const
{
  bool aValid = false;
  SessionPtr aMgr = ModelAPI_Session::get();
  bool isPartSetDocument = aMgr->activeDocument() == aMgr->moduleDocument();

  if (theAttribute->attributeType() == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    // all context objects should be sketch entities
    for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);

      GeomShapePtr aShape = aSelectAttr->value();
      if (!aShape.get()) {
        ResultPtr aResult = aSelectAttr->context();
        if (aResult.get())
          aShape = aResult->shape();
      }
      TopoDS_Shape aTopoShape = aShape->impl<TopoDS_Shape>();
      TopAbs_ShapeEnum aShapeType = aTopoShape.ShapeType();

      if (isPartSetDocument)
        aValid = aShapeType == TopAbs_COMPOUND;
      else {
        TopoDS_Shape aTopoShape = aShape->impl<TopoDS_Shape>();
        aShapeType = GeomValidators_Tools::getCompoundSubType(aTopoShape);

        aValid = aShapeType == TopAbs_SOLID;
      }
    }
  }

  return aValid;
}

