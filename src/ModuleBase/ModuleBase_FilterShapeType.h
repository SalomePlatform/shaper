// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterShapeType.h
// Created:     12 Jan 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_FilterShapeType_H
#define ModuleBase_FilterShapeType_H

#include "ModuleBase.h"

#include "ModuleBase_Filter.h"

#include <StdSelect_TypeOfFace.hxx>

class ModuleBase_FilterShapeType: public ModuleBase_Filter
{
public:
  static MODULEBASE_EXPORT TopAbs_ShapeEnum shapeType(const std::string& theType);

  MODULEBASE_EXPORT ModuleBase_FilterShapeType();

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

private:
  TopAbs_ShapeEnum myShapeType; /// the shape type of the filter
};

#endif //ModuleBase_FilterShapeType
