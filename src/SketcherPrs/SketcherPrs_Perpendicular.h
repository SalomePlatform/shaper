// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Perpendicular.h
// Created:     12 March 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Perpendicular_H
#define SketcherPrs_Perpendicular_H

#include "SketcherPrs_SymbolPrs.h"

class SketchPlugin_Sketch;


DEFINE_STANDARD_HANDLE(SketcherPrs_Perpendicular, SketcherPrs_SymbolPrs)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of perpendicular constraint
*/
class SketcherPrs_Perpendicular: public SketcherPrs_SymbolPrs
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_Perpendicular(SketchPlugin_Constraint* theConstraint, 
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  //! Method which draws selected owners ( for fast presentation draw )
  Standard_EXPORT virtual void HilightSelected(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                               const SelectMgr_SequenceOfOwner& theOwners);
  
  //! Method which hilight an owner belonging to
  //! this selectable object  ( for fast presentation draw )
  Standard_EXPORT virtual void HilightOwnerWithColor(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                                     const Quantity_NameOfColor theColor, const Handle(SelectMgr_EntityOwner)& theOwner);


  DEFINE_STANDARD_RTTI(SketcherPrs_Perpendicular)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

  virtual const char* iconName() const { return "perpendicular.png"; }

private:

  void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

  Handle(Graphic3d_ArrayOfPoints) myPntArray;
};

#endif