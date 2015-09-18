// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Transformation.h
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Transformation_H
#define SketcherPrs_Transformation_H

#include "SketcherPrs_SymbolPrs.h"
#include <ModelAPI_Feature.h>


DEFINE_STANDARD_HANDLE(SketcherPrs_Transformation, SketcherPrs_SymbolPrs)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of parallel constraint
*/
class SketcherPrs_Transformation: public SketcherPrs_SymbolPrs
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \param isTranslation a flag is it translation or rotation
  Standard_EXPORT SketcherPrs_Transformation(ModelAPI_Feature* theConstraint, 
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                         bool isTranslation);

  DEFINE_STANDARD_RTTI(SketcherPrs_Transformation)
protected:
  virtual const char* iconName() const { return myIsTranslation? "translate.png" : "rotate.png"; }

  /// Redefine this function in order to add additiona lines of constraint base
  /// \param thePrs a presentation
  /// \param theColor a color of additiona lines
  virtual void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

  /// Update myPntArray according to presentation positions
  /// \return true in case of success
  virtual bool updatePoints(double theStep) const;

private:
  bool myIsTranslation;
};

#endif