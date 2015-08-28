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
  if (!isPartSetDocument)
    aValid = true;
  else {
    // only hole objects should be selected in PartSet document, there is no decomposition
    if (theAttribute->attributeType() == ModelAPI_AttributeSelectionList::typeId()) {
      AttributeSelectionListPtr aSelectionListAttr = 
                        std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
      for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; i++) {
        AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
        GeomShapePtr aShape = aSelectAttr->value();
        aValid = !aShape.get();
      }
    }
  }

  return aValid;
}

