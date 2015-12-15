/* GeomAPI.i */
%module GeomAPI
%{
  #include "GeomAPI_swig.h"
%}

// to avoid error on this
#define GEOMAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(GeomAPI_AISObject)
%shared_ptr(GeomAPI_Ax1)
%shared_ptr(GeomAPI_Ax2)
%shared_ptr(GeomAPI_Ax3)
%shared_ptr(GeomAPI_Circ)
%shared_ptr(GeomAPI_Circ2d)
%shared_ptr(GeomAPI_Curve)
%shared_ptr(GeomAPI_DataMapOfShapeMapOfShapes)
%shared_ptr(GeomAPI_DataMapOfShapeShape)
%shared_ptr(GeomAPI_Dir)
%shared_ptr(GeomAPI_Dir2d)
%shared_ptr(GeomAPI_Edge)
%shared_ptr(GeomAPI_Face)
%shared_ptr(GeomAPI_ICustomPrs)
%shared_ptr(GeomAPI_Interface)
%shared_ptr(GeomAPI_IPresentable)
%shared_ptr(GeomAPI_Lin)
%shared_ptr(GeomAPI_Lin2d)
%shared_ptr(GeomAPI_PlanarEdges)
%shared_ptr(GeomAPI_Pln)
%shared_ptr(GeomAPI_Pnt)
%shared_ptr(GeomAPI_Pnt2d)
%shared_ptr(GeomAPI_Shape)
%shared_ptr(GeomAPI_ShapeExplorer)
%shared_ptr(GeomAPI_Vertex)
%shared_ptr(GeomAPI_XY)
%shared_ptr(GeomAPI_XYZ)
%shared_ptr(GeomAPI_Trsf)


// all supported interfaces
%include "GeomAPI_Interface.h"
%include "GeomAPI_Shape.h"
%include "GeomAPI_AISObject.h"
%include "GeomAPI_Ax1.h"
%include "GeomAPI_Ax2.h"
%include "GeomAPI_Ax3.h"
%include "GeomAPI_Circ.h"
%include "GeomAPI_Circ2d.h"
%include "GeomAPI_Curve.h"
%include "GeomAPI_DataMapOfShapeMapOfShapes.h"
%include "GeomAPI_DataMapOfShapeShape.h"
%include "GeomAPI_Dir.h"
%include "GeomAPI_Dir2d.h"
%include "GeomAPI_Edge.h"
%include "GeomAPI_Face.h"
%include "GeomAPI_ICustomPrs.h"
%include "GeomAPI_IPresentable.h"
%include "GeomAPI_Lin.h"
%include "GeomAPI_Lin2d.h"
%include "GeomAPI_PlanarEdges.h"
%include "GeomAPI_Pln.h"
%include "GeomAPI_Pnt.h"
%include "GeomAPI_Pnt2d.h"
%include "GeomAPI_ShapeExplorer.h"
%include "GeomAPI_Vertex.h"
%include "GeomAPI_XY.h"
%include "GeomAPI_XYZ.h"
%include "GeomAPI_Trsf.h"
