// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_CustomPrs.hxx
// Created:     10 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_CustomPrs_H
#define XGUI_CustomPrs_H

#include "XGUI.h"
#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Result.h>

/**
* Interface of a class which can provide specific customization of
* object presentation
*/ 
class XGUI_CustomPrs : public GeomAPI_ICustomPrs
{
public:
  XGUI_EXPORT virtual ~XGUI_CustomPrs() {};

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs);
};

#endif
