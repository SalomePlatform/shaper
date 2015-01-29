// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterCustom.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_FilterCustom_H
#define ModuleBase_FilterCustom_H

#include "ModuleBase.h"

#include "ModuleBase_Filter.h"

/**
* \ingroup GUI
* This is a child of ModuleBase_Filter to be used in the factory of filters. Despite of other
* child it does not create the internal filter itself, it get it from the constructor argument.
* This is useful for custom filters, which are not the standard OCC filters. It is not necessary
* to redefine the ModuleBase_Filter. The filter is realized and put here as the class parameter.
*/

class ModuleBase_FilterCustom: public ModuleBase_Filter
{
public:
  /**
   * Constructor
   * \param theFilter an OCC filter to be used in the parent base filter
   */
  MODULEBASE_EXPORT ModuleBase_FilterCustom(Handle(SelectMgr_Filter) theFilter); 

  /**
   * Sets the arguments to the filter. Currently it is not used in this filter.
   * \param theArguments a list of arguments
   */
  MODULEBASE_EXPORT virtual void setArguments(const std::list<std::string>& theArguments);

protected:
  /**
   * It creates an OCC filter. The realization is empty because the filter is set through the constructor
   */
  virtual void createFilter();

};

#endif //ModuleBase_FilterCustom
