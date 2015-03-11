// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Coincident.h
// Created:     12 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Coincident_H
#define SketcherPrs_Coincident_H

#include <GeomAPI_Ax3.h>

#include <AIS_InteractiveObject.hxx>
#include <Standard_DefineHandle.hxx>

class SketchPlugin_Constraint;


DEFINE_STANDARD_HANDLE(SketcherPrs_Coincident, AIS_InteractiveObject)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of coincident constraint
*/
class SketcherPrs_Coincident: public AIS_InteractiveObject
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT SketcherPrs_Coincident(SketchPlugin_Constraint* theConstraint, 
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  Standard_EXPORT virtual void SetColor(const Quantity_Color& aColor);
  
  Standard_EXPORT virtual void SetColor(const Quantity_NameOfColor aColor);
   
  DEFINE_STANDARD_RTTI(SketcherPrs_Coincident)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:
  SketchPlugin_Constraint* myConstraint;
  std::shared_ptr<GeomAPI_Ax3> myPlane;
};


#endif