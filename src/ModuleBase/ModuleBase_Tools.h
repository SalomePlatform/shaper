// File:        ModuleBase_Tools.h
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_Tools_H
#define ModuleBase_Tools_H

#include "ModuleBase.h"

#include <ModelAPI_Result.h>
#include <ModelAPI_Feature.h>

class GeomAPI_Shape;

namespace ModuleBase_Tools {

/**
 * Returns returns a shape if the result has a shape method. Otherwise returns NULL pointer
 */
MODULEBASE_EXPORT boost::shared_ptr<GeomAPI_Shape> shape(ResultPtr theResult);

MODULEBASE_EXPORT FeaturePtr feature(ObjectPtr theObject);

}
;

#endif
