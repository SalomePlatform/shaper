// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
    AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
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
    ModuleBase_Tools::setObject(theAttribute, myObject, myShape, theWorkshop, true, true);
    AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (aRefAttr) {
      if (!myIsObject)
        aRefAttr->setAttr(myRefAttribute);
    }
  }
}
