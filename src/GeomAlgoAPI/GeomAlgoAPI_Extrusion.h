// File:        GeomAlgoAPI_Extrusion.h
// Created:     22 October 2014
// Author:      Sergey Zaritchny

#ifndef GeomAlgoAPI_Extrusion_H_
#define GeomAlgoAPI_Extrusion_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Dir.h>
#include <ModelAPI_ResultBody.h>
#include <boost/shared_ptr.hpp>
/**\class GeomAlgoAPI_Extrusion
 * \ingroup DataAlgo
 * \brief Allows to create the prism based on a given face and a direction
 */

class GeomAlgoAPI_Extrusion : public GeomAPI_Interface
{
 public:

  /* \brief Creates extrusion for the given shape along the normal for this shape
   * \param[in] theShape face or wire to be extruded
   * \param[in] theSize  the length of extrusion (if the value is less than 0, the extrusion in opposite normal)
   * \return a solid or a face which is obtained from specified one
  
  static boost::shared_ptr<GeomAPI_Shape> makeExtrusion(boost::shared_ptr<ModelAPI_ResultBody> theResult,
														boost::shared_ptr<GeomAPI_Shape> theBasis,
														boost::shared_ptr<GeomAPI_Shape> theContext,
                                                        double theSize); */
  /// Constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Extrusion (boost::shared_ptr<GeomAPI_Shape> theBasis, double theSize);

  /// Returns True if algorithm succeed
  GEOMALGOAPI_EXPORT const bool isDone() const;

  ///  Returns True if resulting shape is valid
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// Returns True if resulting shape has volume
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// Returns result of the Extrusion algorithm which may be a Solid or a Face
  GEOMALGOAPI_EXPORT const boost::shared_ptr<GeomAPI_Shape>& shape () const;

  /// Returns list of shapes generated from theShape
  GEOMALGOAPI_EXPORT void generated(const boost::shared_ptr<GeomAPI_Shape> theShape,
                                    ListOfShape& theHistory);

  /// Returns the first shape 
  GEOMALGOAPI_EXPORT const boost::shared_ptr<GeomAPI_Shape>& firstShape();

  /// returns last shape
  GEOMALGOAPI_EXPORT const boost::shared_ptr<GeomAPI_Shape>& lastShape();	 

private:
  /// builds resulting shape
  void build(const boost::shared_ptr<GeomAPI_Shape>& theBasis);

  double mySize;
  bool myDone;
  boost::shared_ptr<GeomAPI_Shape> myShape;
  boost::shared_ptr<GeomAPI_Shape> myFirst;
  boost::shared_ptr<GeomAPI_Shape> myLast;
};

#endif
