// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterMulti.h
// Created:     12 Jan 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_FilterMulti_H
#define ModuleBase_FilterMulti_H

#include "ModuleBase.h"

#include "ModuleBase_Filter.h"

#include <StdSelect_TypeOfFace.hxx>

/**
* An implementation of filter which support using of several filters according to OR statement
*/
class ModuleBase_FilterMulti: public ModuleBase_Filter
{
  /**
   * Creates a new filter according to the given type
   * It is only for internal use.
   * This class creates the sub-filters now. It is possible to use the filter factory for this.
   * \param theType a type of the created filter
   */
  ModuleBase_Filter* findFilter(const std::string& theType);
  
public:
  MODULEBASE_EXPORT ModuleBase_FilterMulti(): 
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

protected:
  std::list<std::string> myArguments; /// the filter arguments
};

#endif //ModuleBase_FilterMulti
