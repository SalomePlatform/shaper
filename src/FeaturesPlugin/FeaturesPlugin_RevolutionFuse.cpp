// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionFuse.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_RevolutionFuse.h"

//=================================================================================================
FeaturesPlugin_RevolutionFuse::FeaturesPlugin_RevolutionFuse()
{
  myFeature = this;
  myOperationType = BOOL_FUSE;
}
