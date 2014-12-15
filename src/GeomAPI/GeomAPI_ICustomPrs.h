// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_ICustomPrs.hxx
// Created:     11 Dec 2014
// Author:      Vitaly SMETANNIKOV

#ifndef GeomAPI_ICustomPrs_H
#define GeomAPI_ICustomPrs_H

#include "GeomAPI_AISObject.h"

/**
* Interface of a class which can provide specific customization of
* object presentation
*/ 
class GeomAPI_ICustomPrs
{
public:
  virtual void customisePresentation(AISObjectPtr thePrs) = 0;
};

typedef std::shared_ptr<GeomAPI_ICustomPrs> GeomCustomPrsPtr;

#endif