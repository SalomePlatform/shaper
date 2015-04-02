// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tangent.h
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Tangent_H
#define SketcherPrs_Tangent_H

#include "SketcherPrs_SymbolPrs.h"
#include <ModelAPI_Feature.h>


DEFINE_STANDARD_HANDLE(SketcherPrs_Tangent, SketcherPrs_SymbolPrs)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of tangent constraint
*/
class SketcherPrs_Tangent: public SketcherPrs_SymbolPrs
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_Tangent(ModelAPI_Feature* theConstraint, 
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  DEFINE_STANDARD_RTTI(SketcherPrs_Tangent)
protected:

  virtual const char* iconName() const { return "tangent.png"; }

  virtual void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

  /// Update myPntArray according to presentation positions
  /// \return true in case of success
  virtual bool updatePoints(double theStep) const;
};

#endif