// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef ModuleBase_ResultPrs_H
#define ModuleBase_ResultPrs_H

#include "ModuleBase.h"

#include <ModelAPI_Result.h>

#include <BRep_Builder.hxx>
#include <NCollection_List.hxx>
#include <ViewerData_AISShape.hxx>
#include <Standard_DefineHandle.hxx>
#include <TopoDS_Compound.hxx>

#include <QMap>

class AIS_ColoredDrawer;
class AIS_InteractiveContext;

DEFINE_STANDARD_HANDLE(ModuleBase_ResultPrs, ViewerData_AISShape)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide specific behaviour 
* for wire presentations based in a one plane
*/
class ModuleBase_ResultPrs: public ViewerData_AISShape
{
public:
  enum SelectionModes {
    Sel_Result = TopAbs_SHAPE + 1 /// it should be combined with Compsolid results, so it is not
                                  /// the Shape type. It is not defined in XML as compound type
                                  /// because this type is processed as shape with compounds inside.
  };

public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT ModuleBase_ResultPrs(ResultPtr theResult);

  //! Method which draws selected owners ( for fast presentation draw )
  Standard_EXPORT virtual void HilightSelected(const Handle(PrsMgr_PresentationManager3d)& thePM,
                                               const SelectMgr_SequenceOfOwner& theOwners);

  //! Method which hilight an owner belonging to
  //! this selectable object  ( for fast presentation draw )
  Standard_EXPORT virtual void HilightOwnerWithColor(
                                        const Handle(PrsMgr_PresentationManager3d)& thePM,
                                        const Handle(Prs3d_Drawer)& theStyle,
                                        const Handle(SelectMgr_EntityOwner)& theOwner);

  /// Returns result object
  Standard_EXPORT ResultPtr getResult() const { return myResult; }

  /// Returns selection priorities that will be added to created selection owner
  /// \return integer value
  Standard_EXPORT int getAdditionalSelectionPriority() const
  { return myAdditionalSelectionPriority; }

  /// Appends a special priority for the mode of selection
  /// \param theSelectionMode a mode of selection, used in ComputeSelection
  /// \param thePriority a new priority value
  Standard_EXPORT void setAdditionalSelectionPriority(const int thePriority);

  //! Updates color of sub shape drawer
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor);

  /// Change presentation to have given shape hidden.
  /// It suports FACE type of the shape to be hidden.
  /// If presentation type is greater than FACE, the SHELL with be shown with the FACE hidden
  /// It is possible to hide more than one FACE by calling the method with given FACES
  /// Visual state of the face is controlled by the second parameter
  /// \param theShapes a container of shape objects
  /// \returns true if the presentation is changed, or false (if for example it was hidden)
  Standard_EXPORT bool setSubShapeHidden(const NCollection_List<TopoDS_Shape>& theShapes);

  /// Returns true if parameter shape has been hidden
  /// \param theShape sub-shape of the presentation shape
  /// \return boolean value
  Standard_EXPORT bool isSubShapeHidden(const TopoDS_Shape& theShape);

  /// Returns true if there are no hidden sub shapes or original shape has at least one not hidden
  /// \param theShapesToSkip container of shape to be hidden in the presentation (faces)
  /// \return boolean value
  Standard_EXPORT bool hasSubShapeVisible(const NCollection_List<TopoDS_Shape>& theShapesToSkip);

  /// Set transparency of hidden sub shapes: if value is 1, shapes are entirely hidden
  /// \param theTransparency transparency value
  /// \return false if parameter is out of [0, 1]
  Standard_EXPORT bool setHiddenSubShapeTransparency(double theTransparency);

  DEFINE_STANDARD_RTTIEXT(ModuleBase_ResultPrs, ViewerData_AISShape)

protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
    const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                const Standard_Integer theMode) ;

private:
  /// If the shape of this presentation is Vertex, it appends custom sensitive and owners for it.
  /// Owner is a usual Brep owner with "isDecomposite" in true. It is necessary to have "Ring"
  /// highlight/selected marker.
  /// \param theSelection a current filled selection
  /// \param theMode a selection mode
  /// \return true if the owner is created
  bool appendVertexSelection(const Handle(SelectMgr_Selection)& aSelection,
                             const Standard_Integer theMode);

  /// Creates compound of vertices, edges and faces.
  /// If the shape is COMPOUND, iterate by sub-shapes.
  /// If the shape is SOLID/SHEL, explore shape by FACES,
  /// If the shape is WIRE, explore shape by EDGES
  /// \param theBuilder result compound builder
  /// \param theCompound the result shape
  /// \param theShape the processed shape
  /// \param theHiddenSubShapes container of shapes to be skipped (faces)
  void collectSubShapes(BRep_Builder& theBuilder, TopoDS_Shape& theCompound,
    const TopoDS_Shape& theShape, const NCollection_List<TopoDS_Shape>& theHiddenSubShapes);

  void setEdgesDefaultColor();

private:
  /// Reference to result object
  ResultPtr myResult;

  /// Original shape of the result object
  TopoDS_Shape myOriginalShape;

  /// Container of original Shape sub shape to be hidden and not selectable
  NCollection_List<TopoDS_Shape> myHiddenSubShapes;
  TopoDS_Compound myHiddenCompound; /// compound of hidden sub shapes
  double myTransparency; ///< transparency of hidden shapes, where 0 - there is no transparency
  Handle(AIS_ColoredDrawer) myHiddenSubShapesDrawer; ///< drawer for hidden sub shapes

  /// selection priority that will be added to the standard
  /// selection priority of the selection entity
  int myAdditionalSelectionPriority;
};


#endif