// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OperationPrs.h
// Created:     01 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationPrs_H
#define PartSet_OperationPrs_H

#include "PartSet.h"

#include <ModelAPI_Result.h>
#include <ModelAPI_Feature.h>

#include <ViewerData_AISShape.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide specific behaviour 
* for wire presentations based in a one plane
*/
class PartSet_OperationPrs : public ViewerData_AISShape
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT PartSet_OperationPrs(FeaturePtr theFeature);

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
  /// Reference to a feature object
  FeaturePtr myFeature;
};


#endif