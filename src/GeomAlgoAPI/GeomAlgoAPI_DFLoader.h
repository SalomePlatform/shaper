// File:        GeomAlgoAPI_DFLoader.h
// Created:     23 October 2014
// Author:      Sergey Zaritchny

#ifndef GeomAlgoAPI_DFLoader_H_
#define GeomAlgoAPI_DFLoader_H_
#include <GeomAlgoAPI.h>
#include <TopoDS_Shape.hxx>

/**\class GeomAlgoAPI_DFLoader
 * \ingroup DataAlgo
 * \brief Defines several static methods useful for Data Framework filling
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_DFLoader 
{
 public:
  /// Refine result
  static const TopoDS_Shape refineResult(const TopoDS_Shape& theShape);
};

#endif