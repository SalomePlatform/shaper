// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_ExternalObjectsMgr.cpp
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva

#include "PartSet_ExternalObjectsMgr.h"
#include "PartSet_Tools.h"

#include <XGUI_Workshop.h>
#include <XGUI_ModuleConnector.h>

#include <SketchPlugin_Feature.h>

#include <QString>

PartSet_ExternalObjectsMgr::PartSet_ExternalObjectsMgr(const std::string& theExternal, const bool theDefaultValue)
: myUseExternal(theDefaultValue)
{
  QString aIsExternal(theExternal.c_str());
  if (!aIsExternal.isEmpty()) {
    QString aStr = aIsExternal.toUpper();
    myUseExternal = (aStr == "TRUE") || (aStr == "YES"); 
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
    aSelectedObject = PartSet_Tools::createFixedObjectByExternal(theShape->impl<TopoDS_Shape>(),
                                                    theSelectedObject, theSketch, theTemporary);
    if (aSelectedObject.get()) {
      if (theTemporary)
        myExternalObjectValidated = aSelectedObject;
      else
        myExternalObjects.append(aSelectedObject);
    }
  }
  return aSelectedObject;
}

//********************************************************************
/*ObjectPtr PartSet_ExternalObjectsMgr::externalObjectValidated(const ObjectPtr& theSelectedObject,
                                                     const GeomShapePtr& theShape,
                                                     const CompositeFeaturePtr& theSketch)
{
  // TODO(nds): unite with externalObject()
  ObjectPtr aSelectedObject = PartSet_Tools::findFixedObjectByExternal(theShape->impl<TopoDS_Shape>(),
                                                             theSelectedObject, theSketch);
  if (!aSelectedObject.get()) {
    // Processing of external (non-sketch) object
    aSelectedObject = PartSet_Tools::createFixedObjectByExternal(theShape->impl<TopoDS_Shape>(),
                                                                 theSelectedObject, theSketch);
    if (aSelectedObject.get())
      myExternalObjectValidated = aSelectedObject;
  }
  return aSelectedObject;
}*/

//********************************************************************
void PartSet_ExternalObjectsMgr::removeExternal(const CompositeFeaturePtr& theSketch,
                                                const FeaturePtr& theFeature,
                                                ModuleBase_IWorkshop* theWorkshop,
                                                const bool theTemporary)
{
  if (theTemporary)
    removeExternalObject(myExternalObjectValidated, theSketch, theFeature, theWorkshop);
  else{
    QObjectPtrList::const_iterator anIt = myExternalObjects.begin(), aLast = myExternalObjects.end();
    for (; anIt != aLast; anIt++) {
      ObjectPtr anObject = *anIt;
      removeExternalObject(anObject, theSketch, theFeature, theWorkshop);
    }
    myExternalObjects.clear();
  }
}

//********************************************************************
void PartSet_ExternalObjectsMgr::removeUnusedExternalObjects(const QObjectPtrList& theIgnoreObjects,
                                                             const CompositeFeaturePtr& theSketch,
                                                             const FeaturePtr& theFeature)
{
  /*
  // TODO(nds): unite with removeExternal(), remove parameters
  QObjectPtrList aUsedExternalObjects;

  QObjectPtrList::const_iterator anIt = myExternalObjects.begin(), aLast = myExternalObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (theIgnoreObjects.contains(anObject))
      aUsedExternalObjects.append(anObject);
    else
      removeExternalObject(anObject, theSketch, theFeature);
  }*/
  myExternalObjects.clear();
  //if (!aUsedExternalObjects.empty())
  //  myExternalObjects = aUsedExternalObjects;
}

//********************************************************************
/*void PartSet_ExternalObjectsMgr::removeExternalValidated(const CompositeFeaturePtr& theSketch,
                                                         const FeaturePtr& theFeature,
                                                         ModuleBase_IWorkshop* theWorkshop)
{
  // TODO(nds): unite with removeExternal(), remove parameters
  removeExternalObject(myExternalObjectValidated, theSketch, theFeature, theWorkshop);
  myExternalObjectValidated = ObjectPtr();
}*/

void PartSet_ExternalObjectsMgr::removeExternalObject(const ObjectPtr& theObject,
                                                      const CompositeFeaturePtr& theSketch,
                                                      const FeaturePtr& theFeature,
                                                      ModuleBase_IWorkshop* theWorkshop)
{
  if (theObject.get()) {
    DocumentPtr aDoc = theObject->document();
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get() != NULL) {
      QObjectPtrList anObjects;
      anObjects.append(aFeature);
      // the external feature should be removed with all references, sketch feature should be ignored
      std::set<FeaturePtr> anIgnoredFeatures;
      // the current feature should be ignored, because it can use the external feature in the
      // attributes and, therefore have a references to it. So, the delete functionality tries
      // to delete this feature. Test case is creation of a constraint on external point,
      // use in this control after an external point, the point of the sketch.
      anIgnoredFeatures.insert(theFeature);
      workshop(theWorkshop)->deleteFeatures(anObjects, anIgnoredFeatures);
    }
  }
}

XGUI_Workshop* PartSet_ExternalObjectsMgr::workshop(ModuleBase_IWorkshop* theWorkshop)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
  return aConnector->workshop();
}
