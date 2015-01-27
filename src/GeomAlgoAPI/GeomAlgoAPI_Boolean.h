// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Boolean.h
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#ifndef GeomAlgoAPI_Boolean_H_
#define GeomAlgoAPI_Boolean_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>

/**\class GeomAlgoAPI_Boolean
 * \ingroup DataAlgo
 * \brief Allows to perform of boolean operations
 */
class GeomAlgoAPI_Boolean : public GeomAPI_Interface
{
 public:
  /**\brief Creates cut boolean operation
   * \param[in] theShape the main shape
   * \param[in] theTool  toole shape for boolean
   * \return a solid as result of operation
   */
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> makeCut(
	                                          std::shared_ptr<GeomAPI_Shape> theShape,
                                            std::shared_ptr<GeomAPI_Shape> theTool);

  /**\brief Creates fuse boolean operation
   * \param[in] theShape the main shape
   * \param[in] theTool  second shape
   * \return a solid as result of operation
   */
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> makeFuse(
	                                          std::shared_ptr<GeomAPI_Shape> theShape,
                                              std::shared_ptr<GeomAPI_Shape> theTool);

  /**\brief Creates common boolean operation
   * \param[in] theObject the main shape
   * \param[in] theTool  second shape
   * \return a solid as result of operation
   */
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> makeCommon(
	                                          std::shared_ptr<GeomAPI_Shape> theObject,
                                              std::shared_ptr<GeomAPI_Shape> theTool);

   enum {
	BOOL_CUT,
    BOOL_FUSE,
    BOOL_COMMON
  };
  /// Constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Boolean (std::shared_ptr<GeomAPI_Shape> theObject,
                                          std::shared_ptr<GeomAPI_Shape> theTool,
										  int theType);

  /// Returns True if algorithm succeed
  GEOMALGOAPI_EXPORT const bool isDone() const;

  ///  Returns True if resulting shape is valid
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// Returns result of the boolean algorithm which may be a Solid or a Face
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape () const; 
 
  /// Returns map of sub-shapes of the result. To be used for History keeping
  GEOMALGOAPI_EXPORT void  mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const;

  /// Return interface for for History processing
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape* makeShape () const;

  ///Destructor
  GEOMALGOAPI_EXPORT  ~GeomAlgoAPI_Boolean();

  private:
  /// builds resulting shape
  void build(std::shared_ptr<GeomAPI_Shape> theObject,
             std::shared_ptr<GeomAPI_Shape> theTool);
  /// fields
  double mySize;
  bool myDone;
  int  myOperation;
  std::shared_ptr<GeomAPI_Shape> myShape;  
  GeomAPI_DataMapOfShapeShape myMap;
  GeomAlgoAPI_MakeShape * myMkShape;
};

#endif
