// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PartSet_WidgetShapeSelector.h"

#include "PartSet_CenterPrs.h"
#include "PartSet_Module.h"
#include "PartSet_SketcherMgr.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_ViewerPrs.h>

#include <Config_WidgetAPI.h>

#include <PartSet_ExternalObjectsMgr.h>
#include <SketchPlugin_Feature.h>

#include <XGUI_Workshop.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_Tools.h>

PartSet_WidgetShapeSelector::PartSet_WidgetShapeSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData)
{
  myUseSketchPlane = theData->getBooleanAttribute("use_sketch_plane", true);
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"),
                                         theData->getProperty("can_create_external"), true);
}

PartSet_WidgetShapeSelector::~PartSet_WidgetShapeSelector()
{
  delete myExternalObjectMgr;
}

//********************************************************************
void PartSet_WidgetShapeSelector::selectionFilters(QIntList& theModuleSelectionFilters,
                                                   SelectMgr_ListOfFilter& theSelectionFilters)
{
  ModuleBase_WidgetShapeSelector::selectionFilters(theModuleSelectionFilters, theSelectionFilters);

  if (!myUseSketchPlane) {
    if (theModuleSelectionFilters.contains(SF_SketchPlaneFilter))
      theModuleSelectionFilters.removeAll(SF_SketchPlaneFilter);
  }
}

//********************************************************************
bool PartSet_WidgetShapeSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = false;
  if (thePrs.get() && thePrs->interactive().get() &&
      thePrs->interactive()->IsKind(STANDARD_TYPE(PartSet_CenterPrs)))
    aValid = true; // we should not check acceptSubShape for such presentation
  else
    aValid = ModuleBase_WidgetShapeSelector::isValidSelectionCustom(thePrs);

  if (aValid) {
    ObjectPtr anObject = myWorkshop->selection()->getResult(thePrs);
    aValid = myExternalObjectMgr->isValidObject(anObject);
  }
  return aValid;
}

void PartSet_WidgetShapeSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                   ObjectPtr& theObject,
                                                   GeomShapePtr& theShape)
{
  ModuleBase_WidgetShapeSelector::getGeomSelection(thePrs, theObject, theShape);

  myExternalObjectMgr->getGeomSelection(thePrs, theObject, theShape,
                                        myWorkshop, sketch(), myIsInValidate);
}

//********************************************************************
void PartSet_WidgetShapeSelector::restoreAttributeValue(const AttributePtr& theAttribute,
                                                        const bool theValid)
{
  ModuleBase_WidgetShapeSelector::restoreAttributeValue(theAttribute, theValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop, true);
}

