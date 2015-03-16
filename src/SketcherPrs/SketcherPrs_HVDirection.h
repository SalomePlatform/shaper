// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_HVDirection.h
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_HVDirection_H
#define SketcherPrs_HVDirection_H

#include "SketcherPrs_SymbolPrs.h"

class SketchPlugin_Constraint;
class SketchPlugin_Sketch;


DEFINE_STANDARD_HANDLE(SketcherPrs_HVDirection, SketcherPrs_SymbolPrs)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of parallel constraint
*/
class SketcherPrs_HVDirection: public SketcherPrs_SymbolPrs
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_HVDirection(SketchPlugin_Constraint* theConstraint, 
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                         bool isHorisontal);

  DEFINE_STANDARD_RTTI(SketcherPrs_HVDirection)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  virtual const char* iconName() const { return myIsHorisontal? "horisontal.png" : "vertical.png"; }

  /// Redefine this function in order to add additiona lines of constraint base
  /// \param thePrs a presentation
  /// \param theColor a color of additiona lines
  virtual void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

private:
  bool myIsHorisontal;
};

#endif