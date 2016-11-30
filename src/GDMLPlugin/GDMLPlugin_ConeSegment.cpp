// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GDMLPlugin_ConeSegment.cpp
// Created:     23 Nov 2016
// Author:      Clarisse Genrault (CEA)

#include <GDMLPlugin_ConeSegment.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>

//=================================================================================================
GDMLPlugin_ConeSegment::GDMLPlugin_ConeSegment() // Nothing to do during instantiation
{
}

//=================================================================================================
void GDMLPlugin_ConeSegment::initAttributes()
{
  data()->addAttribute(GDMLPlugin_ConeSegment::RMIN1_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_ConeSegment::RMAX1_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_ConeSegment::RMIN2_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_ConeSegment::RMAX2_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_ConeSegment::Z_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_ConeSegment::STARTPHI_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_ConeSegment::DELTAPHI_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void GDMLPlugin_ConeSegment::execute()
{
  double aRMin1 = real(GDMLPlugin_ConeSegment::RMIN1_ID())->value();
  double aRMax1 = real(GDMLPlugin_ConeSegment::RMAX1_ID())->value();
  double aRMin2 = real(GDMLPlugin_ConeSegment::RMIN2_ID())->value();
  double aRMax2 = real(GDMLPlugin_ConeSegment::RMAX2_ID())->value();
  double aZ = real(GDMLPlugin_ConeSegment::Z_ID())->value();
  double aStartPhi = real(GDMLPlugin_ConeSegment::STARTPHI_ID())->value();
  double aDeltaPhi = real(GDMLPlugin_ConeSegment::DELTAPHI_ID())->value();

  std::shared_ptr<GeomAlgoAPI_ConeSegment> aConeSegmentAlgo =
    std::shared_ptr<GeomAlgoAPI_ConeSegment>(new GeomAlgoAPI_ConeSegment(aRMin1, aRMax1,
                                                                         aRMin2, aRMax2,
                                                                         aZ,
                                                                         aStartPhi, aDeltaPhi));

  // Check with that the arguments for aConeSegmentAlgo are correct
  if (!aConeSegmentAlgo->check()){
    setError(aConeSegmentAlgo->getError());
    return;
  }

  aConeSegmentAlgo->build();

  // Check if the creation of the cone segment went well
  if(!aConeSegmentAlgo->isDone()) {
    setError(aConeSegmentAlgo->getError());
    return;
  }
  if (!aConeSegmentAlgo->checkValid("Cone Segment builder")) {
    setError(aConeSegmentAlgo->getError());
    return;
  }

  int aResultIndex = 0;
  ResultBodyPtr aResultConeSegment = document()->createBody(data(), aResultIndex);
  loadNamingDS(aConeSegmentAlgo, aResultConeSegment);
  setResult(aResultConeSegment, aResultIndex);
}

//=================================================================================================
void GDMLPlugin_ConeSegment::loadNamingDS(
    std::shared_ptr<GeomAlgoAPI_ConeSegment> theConeSegmentAlgo,
    std::shared_ptr<ModelAPI_ResultBody> theResultConeSegment)
{
  // Load the result
  theResultConeSegment->store(theConeSegmentAlgo->shape());

  // Prepare the naming
  theConeSegmentAlgo->prepareNamingFaces();

  // Insert to faces
  int num = 1;
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
    theConeSegmentAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator
       it=listOfFaces.begin(); it!=listOfFaces.end(); ++it) {
    std::shared_ptr<GeomAPI_Shape> aFace = (*it).second;
    theResultConeSegment->generated(aFace, (*it).first, num++);
  }
}

