// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Coincident.h
// Created:     12 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Coincident_H
#define SketcherPrs_Coincident_H

#include <GeomAPI_Ax3.h>
#include <ModelAPI_Feature.h>

#include <AIS_InteractiveObject.hxx>
#include <Standard_DefineHandle.hxx>


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
  /// \param theConstraint a constraint object
  /// \param thePlane plane of a sketch
  Standard_EXPORT SketcherPrs_Coincident(ModelAPI_Feature* theConstraint, 
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Defines color for the presentation
  /// \param aColor a color object
  Standard_EXPORT virtual void SetColor(const Quantity_Color& aColor);
  
  /// Defines color for the presentation
  /// \param aColor a color name
  Standard_EXPORT virtual void SetColor(const Quantity_NameOfColor aColor);

  /// Returns true if the constraint feature arguments are correcly filled to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  DEFINE_STANDARD_RTTI(SketcherPrs_Coincident)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:
  ModelAPI_Feature* myConstraint;
  std::shared_ptr<GeomAPI_Ax3> myPlane;
  gp_Pnt myPoint;
};


#endif