// File:        GeomAlgoAPI_EdgeBuilder.h
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAlgoAPI_EdgeBuilder_HeaderFile
#define GeomAlgoAPI_EdgeBuilder_HeaderFile

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt.h>
#include <boost/shared_ptr.hpp>

/**\class GeomAlgoAPI_EdgeBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_EdgeBuilder
{
public:
  /// Creates linear edge by two points
  static boost::shared_ptr<GeomAPI_Shape> line(
    boost::shared_ptr<GeomAPI_Pnt> theStart, boost::shared_ptr<GeomAPI_Pnt> theEnd);
};

#endif
