// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OperationPrs.h
// Created:     01 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationPrs_H
#define PartSet_OperationPrs_H

#include "PartSet.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>

#include <ModuleBase_IWorkshop.h>

#include <GeomAPI_Shape.h>

#include <ViewerData_AISShape.hxx>
#include <Standard_DefineHandle.hxx>

#include <QMap>
#include <QList>

DEFINE_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide colored presentation of
* a list of shapes. It contains a shapes where the parameter feature refers. It processes the following
* types of attributes: ModelAPI_AttributeSelectionList, ModelAPI_AttributeRefAttr,
* ModelAPI_AttributeSelection and ModelAPI_AttributeReference.
* The selection for this presentation is absent.
*/
class PartSet_OperationPrs : public ViewerData_AISShape
{
public:
  /// Constructor
  Standard_EXPORT PartSet_OperationPrs(ModuleBase_IWorkshop* theWorkshop);

  /// Returns true if the feature contains attributes, which has references to other features
  /// \param theFeature a feature
  /// \return boolean result
  bool canActivate(const FeaturePtr& theFeature);

  /// Sets the operation feature. It is used in Compute method to group the feature parameter shapes
  /// theFeature a feature
  void setFeature(const FeaturePtr& theFeature);

  /// Returns true if the presentation 
  bool dependOn(const ObjectPtr& theObject);

  // Recompute internal list of shaped dependent on the current feature
  void updateShapes();

  DEFINE_STANDARD_RTTI(PartSet_OperationPrs)

protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
            const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
            const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                const Standard_Integer aMode) ;
private:
  /// Fills the map by the feature object and shapes, which should be visuaziled
  /// Gets the feature attribute, collect objects to whom the attribute refers
  /// \param theObjectShape an output map of objects
  void getFeatureShapes(QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes);

  /// Returns true if the attribute type belong to reference attribute
  /// \param theAttribute an attribute
  /// \return a boolean value
  static bool isSelectionAttribute(const AttributePtr& theAttribute);

private:
  ModuleBase_IWorkshop* myWorkshop;
  FeaturePtr myFeature; /// Reference to a feature object
  QMap<ObjectPtr, QList<GeomShapePtr> > myFeatureShapes;
};


#endif