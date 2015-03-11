// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_ICustomPrs.hxx
// Created:     11 Dec 2014
// Author:      Vitaly SMETANNIKOV

#ifndef GeomAPI_ICustomPrs_H
#define GeomAPI_ICustomPrs_H

#include "GeomAPI.h"
#include "GeomAPI_AISObject.h"
#include "GeomAPI_AISObject.h"

#include <vector>
/**
* Interface of a class which can provide specific customization of
* object presentation
*/ 
class ModelAPI_Result;

class GeomAPI_ICustomPrs
{
public:
  GEOMAPI_EXPORT virtual ~GeomAPI_ICustomPrs();

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(std::shared_ptr<ModelAPI_Result> theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs) = 0;
};

typedef std::shared_ptr<GeomAPI_ICustomPrs> GeomCustomPrsPtr;

#endif
