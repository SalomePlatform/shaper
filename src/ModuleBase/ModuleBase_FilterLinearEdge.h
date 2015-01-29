// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterLinearEdge.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_FilterLinearEdge_H
#define ModuleBase_FilterLinearEdge_H

#include "ModuleBase.h"

#include "ModuleBase_Filter.h"

#include <StdSelect_TypeOfEdge.hxx>

/**
* \ingroup GUI
* A class of a viewer filter by linear edges
*/
class ModuleBase_FilterLinearEdge: public ModuleBase_Filter
{
public:
  /// Convert string to StdSelect_TypeOfFace value
  /// \param theType a string value
  static MODULEBASE_EXPORT StdSelect_TypeOfEdge edgeType(const std::string& theType);

  /// Constructor
  MODULEBASE_EXPORT ModuleBase_FilterLinearEdge(): 
      ModuleBase_Filter() {}

  /**
   * Sets the arguments to the filter.
   * \param theArguments a list of arguments
   */
  MODULEBASE_EXPORT virtual void setArguments(const std::list<std::string>& theArguments);

protected:
  /**
   * It creates an OCC edge filter
   */
  virtual void createFilter();

};

#endif //ModuleBase_FilterLinearEdge
