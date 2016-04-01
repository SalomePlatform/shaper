// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionCut.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_RevolutionCut.h"

//=================================================================================================
FeaturesPlugin_RevolutionCut::FeaturesPlugin_RevolutionCut()
{
  myFeature = this;
  myOperationType = BOOL_CUT;
}
