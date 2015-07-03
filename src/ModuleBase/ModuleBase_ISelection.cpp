// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ModuleBase_ISelection.h"

//********************************************************************
void ModuleBase_ISelection::appendSelected(const QList<ModuleBase_ViewerPrs> theValues,
                                           QList<ModuleBase_ViewerPrs>& theValuesTo)
{
  // collect the objects from the viewer
  QObjectPtrList anExistedObjects;
  QList<ModuleBase_ViewerPrs>::const_iterator aPrsIt = theValuesTo.begin(),
                                              aPrsLast = theValuesTo.end();
  for (; aPrsIt != aPrsLast; aPrsIt++) {
    if ((*aPrsIt).owner() && (*aPrsIt).object())
      anExistedObjects.push_back((*aPrsIt).object());
  }


  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theValues.begin(),
                                              aLast = theValues.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = (*anIt).object();
    if (anObject.get() != NULL && !anExistedObjects.contains(anObject)) {
      theValuesTo.append(ModuleBase_ViewerPrs(anObject, TopoDS_Shape(), NULL));
    }
  }

}

//********************************************************************
ResultPtr ModuleBase_ISelection::getResult(const ModuleBase_ViewerPrs& thePrs)
{
  ResultPtr aResult;

  if (!thePrs.owner().IsNull()) {
    ObjectPtr anObject = getSelectableObject(thePrs.owner());
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  }
  else {
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(thePrs.object());
  }

  return aResult;
}

//********************************************************************
GeomShapePtr ModuleBase_ISelection::getShape(const ModuleBase_ViewerPrs& thePrs)
{
  GeomShapePtr aShape;

  const TopoDS_Shape& aTDSShape = thePrs.shape();
  // if only result is selected, an empty shape is set to the model
  if (aTDSShape.IsNull()) {
  }
  else {
    aShape = GeomShapePtr(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aTDSShape));
    // If the result object is built on the same shape, the result shape here is empty one
    ResultPtr aResult = getResult(thePrs);
    if (aResult.get() && aShape->isEqual(aResult->shape()))
      aShape = GeomShapePtr();
  }
  return aShape;
}

//********************************************************************
QList<ModuleBase_ViewerPrs> ModuleBase_ISelection::getViewerPrs(const QObjectPtrList& theObjects)
{
  QList<ModuleBase_ViewerPrs> aSelectedPrs;
  QObjectPtrList::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (anObject.get() != NULL) {
      aSelectedPrs.append(ModuleBase_ViewerPrs(anObject, TopoDS_Shape(), NULL));
    }
  }
  return aSelectedPrs;
}
