// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File: 		GeomDataAPI_swig.h
// Created: Dec 15, 2015
// Author:  Sergey POKHODENKO

#ifndef SRC_GEOMDATAAPI_GEOMDATAAPI_SWIG_H_
#define SRC_GEOMDATAAPI_GEOMDATAAPI_SWIG_H_

  #include "ModelAPI_swig.h"

  #include "GeomDataAPI.h"
  #include "GeomDataAPI_Point.h"
  #include "GeomDataAPI_Dir.h"
  #include "GeomDataAPI_Point2D.h"

  #include <memory>
  #include <string>
  #include <list>

  template<class T> std::shared_ptr<T> castTo(std::shared_ptr<ModelAPI_Attribute> theObject)
  {
    return std::dynamic_pointer_cast<T>(theObject);
  }

#endif /* SRC_GEOMDATAAPI_GEOMDATAAPI_SWIG_H_ */
