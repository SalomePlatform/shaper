// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ModuleBase_ResultPrs_H
#define ModuleBase_ResultPrs_H

#include "ModuleBase.h"

#include <ModelAPI_Result.h>

#include <ViewerData_AISShape.hxx>
#include <Standard_DefineHandle.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <QMap>

DEFINE_STANDARD_HANDLE(ModuleBase_BRepOwner, StdSelect_BRepOwner)

/**
* \ingroup GUI
* A redefinition of standard BRep Owner in order to provide specific selection
* of CompSolid objects. This owner is created only for selection mode TopAbs_COMPSOLID
*/
class ModuleBase_BRepOwner: public StdSelect_BRepOwner
{
public:
  /// Constructor
  /// \param aShape an owner shape
  /// \param aPriority drawig priority
  /// \param ComesFromDecomposition decomposition flag
  ModuleBase_BRepOwner(const TopoDS_Shape& aShape,
    const Standard_Integer aPriority = 0,
    const Standard_Boolean ComesFromDecomposition = Standard_False)
    : StdSelect_BRepOwner(aShape, aPriority, ComesFromDecomposition) {}

  /// Highlight the presentation with the given color
  /// \param aPM a presentations manager
  /// \param theStyle a style of presentation
  /// \param aMode a drawing mode
  virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& aPM,
    const Handle(Graphic3d_HighlightStyle)& theStyle, const Standard_Integer aMode = 0)
  {
    Selectable()->HilightOwnerWithColor(aPM, theStyle, this);
  }

  DEFINE_STANDARD_RTTIEXT(ModuleBase_BRepOwner, StdSelect_BRepOwner)
};



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
                                        const Handle(Graphic3d_HighlightStyle)& theStyle,
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

  DEFINE_STANDARD_RTTIEXT(ModuleBase_ResultPrs, ViewerData_AISShape)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
    const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:
  /// Appens sensitive and owners for wires of the given shape into selection
  /// \param theSelection a current filled selection
  /// \param theShape a shape
  void appendWiresSelection(const Handle(SelectMgr_Selection)& theSelection,
                            const TopoDS_Shape& theShape);

  /// Reference to result object
  ResultPtr myResult;

  /// Original shape of the result object
  TopoDS_Shape myOriginalShape;

  /// selection priority that will be added to the standard
  /// selection priority of the selection entity
  int myAdditionalSelectionPriority;
};


#endif