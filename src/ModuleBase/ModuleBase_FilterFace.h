// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterFace.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_FilterFace_H
#define ModuleBase_FilterFace_H

#include "ModuleBase.h"

#include "ModuleBase_Filter.h"

#include <StdSelect_TypeOfFace.hxx>

/**
* A class of a viewer filter by faces
*/
class ModuleBase_FilterFace: public ModuleBase_Filter
{
public:
  /// Convert string to StdSelect_TypeOfFace value
  /// \param theType a string value
  static MODULEBASE_EXPORT StdSelect_TypeOfFace faceType(const std::string& theType);

  /// Constructor
  MODULEBASE_EXPORT ModuleBase_FilterFace(): 
      ModuleBase_Filter() {}

  /**
   * Sets the arguments to the filter.
   * \param theArguments a list of arguments
   */
  MODULEBASE_EXPORT virtual void setArguments(const std::list<std::string>& theArguments);

protected:
  /**
   * It creates an OCC face filter
   */
  virtual void createFilter();

};

#endif //ModuleBase_FilterFace
