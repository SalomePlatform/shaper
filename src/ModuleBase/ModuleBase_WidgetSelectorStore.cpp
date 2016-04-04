// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetSelectorStore.cpp
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetSelectorStore.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>

ModuleBase_WidgetSelectorStore::ModuleBase_WidgetSelectorStore()
: myIsObject(false), mySelectionType(""), mySelectionCount(0)
{
}

void ModuleBase_WidgetSelectorStore::storeAttributeValue(const AttributePtr& theAttribute,
                                                         ModuleBase_IWorkshop* theWorkshop)
{
  if (!theAttribute.get())
    return;

  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr =
                         std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    mySelectionType = aSelectionListAttr->selectionType();
    mySelectionCount = aSelectionListAttr->size();
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr =
                         std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    mySelectionCount = aRefListAttr->size();
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr =
                         std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
    mySelectionCount = aRefAttrListAttr->size();
  }
  else {
    myObject = ModuleBase_Tools::getObject(theAttribute);
    myShape = ModuleBase_Tools::getShape(theAttribute, theWorkshop);
    myRefAttribute = AttributePtr();
    myIsObject = false;
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (aRefAttr) {
      myIsObject = aRefAttr->isObject();
      myRefAttribute = aRefAttr->attr();
    }
  }
}

//********************************************************************
void ModuleBase_WidgetSelectorStore::restoreAttributeValue(const AttributePtr& theAttribute,
                                                           ModuleBase_IWorkshop* theWorkshop)
{
  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                   std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    aSelectionListAttr->setSelectionType(mySelectionType);
    // restore selection in the attribute. Indeed there is only one stored object
    int aCountAppened = aSelectionListAttr->size() - mySelectionCount;
    for (int i = 0; i < aCountAppened; i++)
      aSelectionListAttr->removeLast();
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = 
                         std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    // restore objects in the attribute. Indeed there is only one stored object
    int aCountAppened = aRefListAttr->size() - mySelectionCount;
    for (int i = 0; i < aCountAppened; i++)
      aRefListAttr->removeLast();
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr =
                     std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
    // restore objects in the attribute. Indeed there is only one stored object
    int aCountAppened = aRefAttrListAttr->size() - mySelectionCount;
    for (int i = 0; i < aCountAppened; i++)
      aRefAttrListAttr->removeLast();
  }
  else {
    ModuleBase_Tools::setObject(theAttribute, myObject, myShape, theWorkshop, true);
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (aRefAttr) {
      if (!myIsObject)
        aRefAttr->setAttr(myRefAttribute);
    }
  }
}
