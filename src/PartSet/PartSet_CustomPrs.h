// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CustomPrs.hxx
// Created:     30 Jun 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_CustomPrs_H
#define PartSet_CustomPrs_H

#include "PartSet.h"
#include "PartSet_OperationPrs.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Feature.h>

#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_AISObject.h>
#include <GeomAPI_Shape.h>

#include <QMap>
#include <QList>

class ModuleBase_IWorkshop;

/**
* Interface of a class which can provide specific customization of
* object presentation
*/
class PartSet_CustomPrs : public GeomAPI_ICustomPrs
{
public:
  PARTSET_EXPORT PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop);
  PARTSET_EXPORT virtual ~PartSet_CustomPrs() {};

  /// Set the feature is customized
  /// \param theObject a feature object
  void setCustomized(const FeaturePtr& theObject);

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs);
private:
  ModuleBase_IWorkshop* myWorkshop; /// current workshop
  Handle(PartSet_OperationPrs) myOperationPrs; /// AIS presentation for the feature of operation
  //QMap<ResultPtr, QList<GeomShapePtr> > myCustomized; /// objects, which are customized
};

#endif
