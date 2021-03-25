// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef PartSet_ResultSketchPrs_H
#define PartSet_ResultSketchPrs_H

#include "ModuleBase.h"

#include <ModelAPI_Result.h>

#include <ViewerData_AISShape.hxx>

#include <TopoDS_Compound.hxx>

DEFINE_STANDARD_HANDLE(PartSet_ResultSketchPrs, ViewerData_AISShape)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide specific behaviour 
* for wire presentations based in a one plane
*/
class PartSet_ResultSketchPrs: public ViewerData_AISShape
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT PartSet_ResultSketchPrs(ResultPtr theResult);

  DEFINE_STANDARD_RTTIEXT(PartSet_ResultSketchPrs, ViewerData_AISShape)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
    const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer theMode) ;

private:
  /// Appens sensitive and owners for wires of the given shape into selection
  /// \param theSelection a current filled selection
  /// \param theShape a shape
  /// \param theTypeOfSelection type of selection: VERTEX, EDGE, WIRE
  void appendShapeSelection(const Handle(SelectMgr_Selection)& theSelection,
                            const TopoDS_Shape& theShape,
                            const TopAbs_ShapeEnum& theTypeOfSelection);

  /// Sets color/line style/width of the presentation depending on the parameter state
  /// \param isAuxiliary a boolean value if the properties are for auxiliary objects
  void setAuxiliaryPresentationStyle(const bool isAuxiliary);

  /// Fills the containers by the current result
  /// \param theResultShape contains a shape of the result, it will be set as a Shape of AIS_Shape
  /// \param theAuxiliaryCompound a compound of auxiliary shapes
  /// \param theFaceList a list of face shapes
  void fillShapes(TopoDS_Shape& aResultShape,
                  TopoDS_Compound& theAuxiliaryCompound,
                  NCollection_List<TopoDS_Shape>& theFaceList);

  /// Reference to result object
  ResultPtr myResult;

  /// Original shape of the result object
  TopoDS_Shape myOriginalShape;

  /// Compound of auxiliary shapes
  TopoDS_Compound myAuxiliaryCompound;

  /// List of faces
  //std::list<std::shared_ptr<GeomAPI_Shape> > myFacesList;
  NCollection_List<TopoDS_Shape> mySketchFaceList;
};


#endif