// Copyright (C) 2014-2016 CEA/DEN, EDF R&D -->

// File:        GDMLAPI_ConeSegment.cpp
// Created:     29 Nov. 2016
// Author:      Clarisse Genrault

#include "GDMLAPI_ConeSegment.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
GDMLAPI_ConeSegment::GDMLAPI_ConeSegment(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
GDMLAPI_ConeSegment::GDMLAPI_ConeSegment(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                         const ModelHighAPI_Double& theRMin1,
                                         const ModelHighAPI_Double& theRMax1,
                                         const ModelHighAPI_Double& theRMin2,
                                         const ModelHighAPI_Double& theRMax2,
                                         const ModelHighAPI_Double& theZ,
                                         const ModelHighAPI_Double& theStartPhi,
                                         const ModelHighAPI_Double& theDeltaPhi)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setAttributes(theRMin1, theRMax1, theRMin2, theRMax2, theZ, theStartPhi, theDeltaPhi);
}

//==================================================================================================
GDMLAPI_ConeSegment::~GDMLAPI_ConeSegment()
{

}

//==================================================================================================
void GDMLAPI_ConeSegment::setAttributes(const ModelHighAPI_Double& theRMin1,
                                        const ModelHighAPI_Double& theRMax1,
                                        const ModelHighAPI_Double& theRMin2,
                                        const ModelHighAPI_Double& theRMax2,
                                        const ModelHighAPI_Double& theZ,
                                        const ModelHighAPI_Double& theStartPhi,
                                        const ModelHighAPI_Double& theDeltaPhi)
{
  fillAttribute(theRMin1, rmin1());
  fillAttribute(theRMax1, rmax1());
  fillAttribute(theRMin2, rmin2());
  fillAttribute(theRMax2, rmax2());
  fillAttribute(theZ, z());
  fillAttribute(theStartPhi, startphi());
  fillAttribute(theDeltaPhi, deltaphi());

  execute();
}

//==================================================================================================
ConeSegmentPtr addConeSegment(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Double& theRMin1,
                              const ModelHighAPI_Double& theRMax1,
                              const ModelHighAPI_Double& theRMin2,
                              const ModelHighAPI_Double& theRMax2,
                              const ModelHighAPI_Double& theZ,
                              const ModelHighAPI_Double& theStartPhi,
                              const ModelHighAPI_Double& theDeltaPhi)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(GDMLAPI_ConeSegment::ID());
  return ConeSegmentPtr(new GDMLAPI_ConeSegment(aFeature, theRMin1, theRMax1, theRMin2, theRMax2,
                                                theZ, theStartPhi, theDeltaPhi));
}
