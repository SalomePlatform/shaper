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
  Standard_EXPORT SketcherPrs_Perpendicular(ModelAPI_Feature* theConstraint, 
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  DEFINE_STANDARD_RTTI(SketcherPrs_Perpendicular)

  /// Returns true if the constraint feature arguments are correcly filled to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);
protected:
  virtual const char* iconName() const { return "perpendicular.png"; }

  /// Redefine this function in order to add additiona lines of constraint base
  /// \param thePrs a presentation
  /// \param theColor a color of additiona lines
  virtual void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

  /// Update myPntArray according to presentation positions
  /// \return true in case of success
  virtual bool updatePoints(double theStep) const;
};

#endif