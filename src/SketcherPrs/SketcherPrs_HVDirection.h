// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_HVDirection.h
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_HVDirection_H
#define SketcherPrs_HVDirection_H

#include "SketcherPrs_SymbolPrs.h"
#include <ModelAPI_Feature.h>


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
  /// \param isHorisontal a flag horizontal or vertical presentation
  Standard_EXPORT SketcherPrs_HVDirection(ModelAPI_Feature* theConstraint, 
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                         bool isHorisontal);

  DEFINE_STANDARD_RTTI(SketcherPrs_HVDirection)

  /// Returns true if the constraint feature arguments are correcly filled to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);

protected:
  virtual const char* iconName() const { return myIsHorisontal? "horisontal.png" : "vertical.png"; }

  /// Redefine this function in order to add additiona lines of constraint base
  /// \param thePrs a presentation
  /// \param theColor a color of additiona lines
  virtual void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

  /// Update myPntArray according to presentation positions
  /// \return true in case of success
  virtual bool updateIfReadyToDisplay(double theStep) const;

private:
  bool myIsHorisontal;
};

#endif