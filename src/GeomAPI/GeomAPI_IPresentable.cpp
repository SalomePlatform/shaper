// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
/*
 *  Created on: Jul 10, 2015
 *      Author: vsv
 */
// This file is added in order to avoid a problem with
// dynamic_cast of GeomAPI_IPresentable objects under
// SALOME in release mode



#include "GeomAPI_IPresentable.h"


GeomAPI_IPresentable::~GeomAPI_IPresentable()
{
}
