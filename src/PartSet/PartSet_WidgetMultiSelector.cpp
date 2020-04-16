// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "PartSet_WidgetMultiSelector.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <Config_WidgetAPI.h>

#include <PartSet_CenterPrs.h>
#include <PartSet_Tools.h>
#include <PartSet_ExternalObjectsMgr.h>
#include <SketchPlugin_Feature.h>

#include <SketchPlugin_ConstraintRigid.h>

#include <XGUI_Workshop.h>

#include <QComboBox>

PartSet_WidgetMultiSelector::PartSet_WidgetMultiSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetMultiSelector(theParent, theWorkshop, theData)
{
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"),
                                        theData->getProperty("can_create_external"), false);
}

PartSet_WidgetMultiSelector::~PartSet_WidgetMultiSelector()
{
  delete myExternalObjectMgr;
}

//********************************************************************
bool PartSet_WidgetMultiSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = false;
  if (thePrs.get() && thePrs->interactive().get() &&
      thePrs->interactive()->IsKind(STANDARD_TYPE(PartSet_CenterPrs)))
    aValid = true; // we should not check acceptSubShape for such presentation
  else
    aValid = ModuleBase_WidgetMultiSelector::isValidSelectionCustom(thePrs);

  if (aValid) {
    ObjectPtr anObject = myWorkshop->selection()->getResult(thePrs);
    aValid = myExternalObjectMgr->isValidObject(anObject);
  }
  return aValid;
}

//********************************************************************
void PartSet_WidgetMultiSelector::restoreAttributeValue(const AttributePtr& theAttribute,
                                                        const bool theValid)
{
  ModuleBase_WidgetMultiSelector::restoreAttributeValue(theAttribute, theValid);

  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop, true);
}

void PartSet_WidgetMultiSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                   ObjectPtr& theObject,
                                                   GeomShapePtr& theShape)
{
  ModuleBase_WidgetMultiSelector::getGeomSelection(thePrs, theObject, theShape);

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theObject);
  std::shared_ptr<SketchPlugin_Feature> aSPFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  // there is no a sketch feature is selected, but the shape exists, try to create an exernal object
  // TODO: unite with the same functionality in PartSet_WidgetShapeSelector
  if (aSPFeature.get() == NULL) {
    ObjectPtr anExternalObject = ObjectPtr();
    GeomShapePtr anExternalShape = GeomShapePtr();
    if (myExternalObjectMgr->useExternal()) {
      if (myExternalObjectMgr->canCreateExternal()) {
        GeomShapePtr aShape = theShape;
        if (!aShape.get()) {
          ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
          if (aResult.get())
            aShape = aResult->shape();
        }
        if (aShape.get() != NULL && !aShape->isNull())
          anExternalObject =
            myExternalObjectMgr->externalObject(theObject, aShape, sketch(), myIsInValidate);
      }
      else {
        anExternalObject = theObject;
        anExternalShape = theShape;
      }
    }
    /// the object is null if the selected feature is "external"(not sketch entity feature of the
    /// current sketch) and it is not created by object manager
    theObject = anExternalObject;
    theShape = anExternalShape;
  }
}
