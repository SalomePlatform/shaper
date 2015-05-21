// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ModuleBase_ISelection.h"

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
