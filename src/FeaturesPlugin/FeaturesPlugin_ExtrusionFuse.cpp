// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionFuse.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_ExtrusionFuse.h"

//=================================================================================================
FeaturesPlugin_ExtrusionFuse::FeaturesPlugin_ExtrusionFuse()
{
  myFeature = this;
  myOperationType = BOOL_FUSE;
}

//=================================================================================================
void FeaturesPlugin_ExtrusionFuse::execute()
{
  executeCompositeBoolean();
}
