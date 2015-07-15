// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_XYZ.hxx
// Created:     13 July 2015
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Trsf_H_
#define GeomAPI_Trsf_H_

#include <GeomAPI_Interface.h>
#include <memory>

/**\class GeomAPI_Trsf
 * \ingroup DataModel
 * \brief Keep the transformation matrix coefficients
 */

class GeomAPI_Trsf : public GeomAPI_Interface
{
 public:
  /// Keeps no transformation, it may be set by setImpl
  GEOMAPI_EXPORT GeomAPI_Trsf();
  /// Takes the pointer to existing transformation
  GEOMAPI_EXPORT GeomAPI_Trsf(void* theTrsf);
};

#endif

