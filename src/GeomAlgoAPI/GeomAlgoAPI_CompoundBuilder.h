// File:        GeomAlgoAPI_CompoundBuilder.h
// Created:     24 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef GeomAlgoAPI_CompoundBuilder_HeaderFile
#define GeomAlgoAPI_CompoundBuilder_HeaderFile

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <boost/shared_ptr.hpp>

#include <list>

/**\class GeomAlgoAPI_CompoundBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_CompoundBuilder
{
public:
  /// Creates compund of the given shapes
  /// \param theShapes a list of shapes
  static boost::shared_ptr<GeomAPI_Shape> compound
                            (std::list<boost::shared_ptr<GeomAPI_Shape> > theShapes);
};

#endif
