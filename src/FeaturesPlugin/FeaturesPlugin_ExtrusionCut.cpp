// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionCut.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_ExtrusionCut.h"

//=================================================================================================
FeaturesPlugin_ExtrusionCut::FeaturesPlugin_ExtrusionCut()
{
  myFeature = this;
  myOperationType = BOOL_CUT;
}
