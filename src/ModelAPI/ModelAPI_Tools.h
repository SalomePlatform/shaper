// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.h
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#ifndef ModelAPI_Tools_HeaderFile
#define ModelAPI_Tools_HeaderFile

#include "ModelAPI.h"
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <GeomAPI_Shape.h>

#include <vector>

namespace ModelAPI_Tools {
/// Returns shape from the given Result object
MODELAPI_EXPORT std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult);

/*!
 * Searches for variable with name \param theName in the active document (Part), when
 * in the root document (PartSet). If found, set it value in the \param outValue
 * and returns true.
 */
MODELAPI_EXPORT bool findVariable(const std::string& theName, double& outValue, 
  ResultParameterPtr& theParam);

/*!
 * Returns the values of the next random color. The values are in range [0, 255]
 * \param theValues a container of component of RGB value: red, green, blue
 */
MODELAPI_EXPORT void findRandomColor(std::vector<int>& theValues);

}

#endif
