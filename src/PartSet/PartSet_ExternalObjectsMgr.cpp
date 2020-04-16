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

#include "PartSet_ExternalObjectsMgr.h"

#include "PartSet_CenterPrs.h"
#include "PartSet_Tools.h"

#include <XGUI_Workshop.h>
#include <XGUI_ModuleConnector.h>

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_ISelection.h>

#include <SketchPlugin_Feature.h>

#include <QString>

PartSet_ExternalObjectsMgr::PartSet_ExternalObjectsMgr(const std::string& theExternal,
                                                       const std::string& theCanCreateExternal,
                                                       const bool theDefaultValue)
: myUseExternal(theDefaultValue), myCanCreateExternal(true)
{
  QString aIsExternal(theExternal.c_str());
  if (!aIsExternal.isEmpty()) {
    QString aStr = aIsExternal.toUpper();
    myUseExternal = (aStr == "TRUE") || (aStr == "YES");
  }

  QString aCanCreateExternal(theCanCreateExternal.c_str());
  if (!aCanCreateExternal.isEmpty()) {
    QString aStr = aCanCreateExternal.toUpper();
    myCanCreateExternal = (aStr == "TRUE") || (aStr == "YES");
  }
}

bool PartSet_ExternalObjectsMgr::isValidObject(const ObjectPtr& theObject)
{
  bool aValid = true;
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  // Do check using of external feature
  std::shared_ptr<SketchPlugin_Feature> aSPFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);

  // Do check that we can use external feature
  if (aSPFeature.get() != NULL && aSPFeature->isExternal() && !useExternal()) {
    aValid = false;
  }

  return aValid;
}

ObjectPtr PartSet_ExternalObjectsMgr::externalObject(const ObjectPtr& theSelectedObject,
                                                     const GeomShapePtr& theShape,
                                                     const CompositeFeaturePtr& theSketch,
                                                     const bool theTemporary)
{
  ObjectPtr aSelectedObject = PartSet_Tools::findFixedObjectByExternal(
                                  theShape->impl<TopoDS_Shape>(), theSelectedObject, theSketch);
  if (!aSelectedObject.get()) {
    // Processing of external (non-sketch) object
    FeaturePtr aCreatedFeature;
    aSelectedObject = PartSet_Tools::createFixedObjectByExternal(theShape,
      theSelectedObject, theSketch, theTemporary, aCreatedFeature);
    if (aCreatedFeature.get() && theTemporary)
        myExternalObjectValidated = aCreatedFeature;
  }
  return aSelectedObject;
}

ObjectPtr PartSet_ExternalObjectsMgr::externalCenterObject(const ModuleBase_ViewerPrsPtr& thePrs,
                                                           const CompositeFeaturePtr& theSketch,
                                                           const bool theTemporary)
{
  if (!thePrs.get() || thePrs->interactive().IsNull())
    return ObjectPtr();

  Handle(PartSet_CenterPrs) aAIS = Handle(PartSet_CenterPrs)::DownCast(thePrs->interactive());
  if (aAIS.IsNull())
    return ObjectPtr();

  gp_Pnt aPntComp = aAIS->Component()->Pnt();
  GeomVertexPtr aVertPtr(new GeomAPI_Vertex(aPntComp.X(), aPntComp.Y(), aPntComp.Z()));
  TopoDS_Shape aShape = aVertPtr->impl<TopoDS_Shape>();

  ResultPtr aSelectedObject =
    PartSet_Tools::findFixedObjectByExternal(aShape, aAIS->object(), theSketch);
  if (!aSelectedObject.get())
  {
    FeaturePtr aCreatedFeature;
    aSelectedObject = PartSet_Tools::createFixedByExternalCenter(aAIS->object(), aAIS->edge(),
      aAIS->centerType(), theSketch, theTemporary, aCreatedFeature);
    if (aCreatedFeature.get() && theTemporary)
        myExternalObjectValidated = aCreatedFeature;
  }
  return aSelectedObject;
}

void PartSet_ExternalObjectsMgr::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                  ObjectPtr& theObject,
                                                  GeomShapePtr& theShape,
                                                  ModuleBase_IWorkshop* theWorkshop,
                                                  const CompositeFeaturePtr& theSketch,
                                                  const bool isInValidate)
{
  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theObject);
  std::shared_ptr<SketchPlugin_Feature> aSPFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  // there is no a sketch feature is selected, but the shape exists,
  // try to create an exernal object
  // TODO: unite with the same functionality in PartSet_WidgetShapeSelector
  if (aSPFeature.get() == NULL) {
    ObjectPtr anExternalObject = ObjectPtr();
    GeomShapePtr anExternalShape = GeomShapePtr();
    if (useExternal()) {
      if (canCreateExternal()) {
        GeomShapePtr aShape = theShape;
        if (!aShape.get()) {
          ResultPtr aResult = theWorkshop->selection()->getResult(thePrs);
          if (aResult.get())
            aShape = aResult->shape();
        }
        if (aShape.get() != NULL && !aShape->isNull())
          anExternalObject =
            externalObject(theObject, aShape, theSketch, isInValidate);
        if (!anExternalObject.get()) {
          anExternalObject = externalCenterObject(thePrs, theSketch, isInValidate);
        }
      }
      else { /// use objects of found selection
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

//********************************************************************
void PartSet_ExternalObjectsMgr::removeExternal(const CompositeFeaturePtr& theSketch,
                                                const FeaturePtr& theFeature,
                                                ModuleBase_IWorkshop* theWorkshop,
                                                const bool theTemporary)
{
  if (theTemporary)
    removeExternalObject(myExternalObjectValidated, theSketch, theFeature, theWorkshop);
}

void PartSet_ExternalObjectsMgr::removeExternalObject(const ObjectPtr& theObject,
                                                      const CompositeFeaturePtr& /*theSketch*/,
                                                      const FeaturePtr& theFeature,
                                                      ModuleBase_IWorkshop* theWorkshop)
{
  if (theObject.get()) {
    DocumentPtr aDoc = theObject->document();
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get() != NULL) {
      QObjectPtrList anObjects;
      anObjects.append(aFeature);
      // the external feature should be removed with all references,
      // composite sketch feature will be ignored
      workshop(theWorkshop)->deleteFeatures(anObjects);
    }
  }
}

XGUI_Workshop* PartSet_ExternalObjectsMgr::workshop(ModuleBase_IWorkshop* theWorkshop)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
  return aConnector->workshop();
}
