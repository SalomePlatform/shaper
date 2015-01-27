// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Filter.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_Filter_H
#define ModuleBase_Filter_H

#include "ModuleBase.h"

#include <SelectMgr_Filter.hxx>

#include <list>

/**
 * This object is assigned by the name
 * in the XML file to the specific attribute or to the whole feature.
 * If isOK method of the filter returns "false", it is signalized in user interface
 * that the processed entity should not be selected.
 * Filterss must be registered in the filters factory to be
 * correctly identified by the XML string-ID.
 */
class ModuleBase_Filter
{
public:
  /// Constructor
  MODULEBASE_EXPORT ModuleBase_Filter() {}

  /**
   * Returns an OCC selection filter. It can be appended into the context of a viewer
   * It creates a filter if it has not be created yet.
   * \return the selection filter
   */
  MODULEBASE_EXPORT Handle(SelectMgr_Filter) getFilter();

  /**
   * Sets the arguments to the filter.
   * \param theArguments a list of arguments
   */
  MODULEBASE_EXPORT virtual void setArguments(const std::list<std::string>& theArguments) = 0;
protected:
  /**
   * It creates an internal instance of the OCC filter
   */
  virtual void createFilter() = 0;

protected:
  /// current instance of the OCC selection filter
  Handle(SelectMgr_Filter) myFilter; 
};

#endif //ModuleBase_Filter
