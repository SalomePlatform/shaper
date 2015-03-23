// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Tools.h
// Created:     23 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_Tools_HeaderFile
#define GeomValidators_Tools_HeaderFile

#include "GeomValidators.h"
#include "ModelAPI_Object.h"
#include "ModelAPI_Attribute.h"


namespace GeomValidators_Tools
{
  // Returns the object from the attribute
  /// \param theObj an object 
  GEOMVALIDATORS_EXPORT ObjectPtr getObject(const AttributePtr& theAttribute);

};

#endif
