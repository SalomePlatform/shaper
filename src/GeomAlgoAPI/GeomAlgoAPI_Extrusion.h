// File:        GeomAlgoAPI_Extrusion.h
// Created:     06 Jun 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAlgoAPI_Extrusion_HeaderFile
#define GeomAlgoAPI_Extrusion_HeaderFile

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Dir.h>
#include <boost/shared_ptr.hpp>

/**\class GeomAlgoAPI_Extrusion
 * \ingroup DataAlgo
 * \brief Allows to create the prism based on a given face and a direction
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_Extrusion
{
public:
  /* \brief Creates extrusion for the given shape
   * \param[in] theShape face or wire to be extruded
   * \param[in] theDir   direction of extrusion
   * \param[in] theSize  the length of extrusion (if the value is less than 0, the extrusion in opposite direction)
   * \return a solid or a face which is obtained from specified one
   */
  static boost::shared_ptr<GeomAPI_Shape> makeExtrusion(
                        boost::shared_ptr<GeomAPI_Shape> theShape,
                        boost::shared_ptr<GeomAPI_Dir>   theDir,
                        double                           theSize);

  /* \brief Creates extrusion for the given shape along the normal for this shape
   * \param[in] theShape face or wire to be extruded
   * \param[in] theSize  the length of extrusion (if the value is less than 0, the extrusion in opposite normal)
   * \return a solid or a face which is obtained from specified one
   */
  static boost::shared_ptr<GeomAPI_Shape> makeExtrusion(
                        boost::shared_ptr<GeomAPI_Shape> theShape,
                        double                           theSize);
};

#endif
