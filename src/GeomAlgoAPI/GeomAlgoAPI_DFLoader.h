// File:        GeomAlgoAPI_DFLoader.h
// Created:     23 October 2014
// Author:      Sergey Zaritchny

#ifndef GeomAlgoAPI_DFLoader_H_
#define GeomAlgoAPI_DFLoader_H_
#include <GeomAlgoAPI.h>
//#include <boost/shared_ptr.hpp>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

/**\class GeomAlgoAPI_DFLoader
 * \ingroup DataAlgo
 * \brief Defines several static methods useful for Data Framework filling
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_DFLoader 
{
 public:
	 /*
  /// Loads to DF deleted shapes
  static void loadDeletedShapes (BRepBuilderAPI_MakeShape& theMS, const TopoDS_Shape& theShapeIn,
                                 const TopAbs_ShapeEnum  KindOfShape, TNaming_Builder&  theBuilder);

  /// Loads to DF generated shapes
  static void loadAndOrientGeneratedShapes (BRepBuilderAPI_MakeShape&                  theMS,
                                            const TopoDS_Shape&                 theShapeIn,
                                            const TopAbs_ShapeEnum              theKindOfShape,
                                            TNaming_Builder&                    theBuilder,
                                            const TopTools_DataMapOfShapeShape& theSubShapes);
  /// Loads to DF modified shapes 
  static void loadAndOrientModifiedShapes (BRepBuilderAPI_MakeShape&                   theMS,
                                           const TopoDS_Shape&                  theShapeIn,
                                           const TopAbs_ShapeEnum               theKindOfShape,
                                           TNaming_Builder&                     theBuilder,
                                           const TopTools_DataMapOfShapeShape&  theSubShapes);
  */
  /// Refine result
  static const TopoDS_Shape refineResult(const TopoDS_Shape& theShape);
};

#endif