// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  File:   GEOMAlgo_Splitter.hxx
//
//  Author: Peter KURNEV

#ifndef GEOMAlgo_Splitter_HeaderFile
#define GEOMAlgo_Splitter_HeaderFile

#include <GeomAlgoImpl.h>

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>

#include <NCollection_BaseAllocator.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS_Shape.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>

#include <BOPAlgo_Builder.hxx>

//=======================================================================
//class    : GEOMAlgo_Splitter
//purpose  :
//=======================================================================
/*!
 * \class GEOMAlgo_Splitter
 * \ingroup DataAlgo
 * A class for shapes partitioning
 */
 class GEOMAlgo_Splitter : public BOPAlgo_Builder
{
 public:
  /// Constructor
  GEOMALGOIMPL_EXPORT GEOMAlgo_Splitter();

  /// Constructor
  /// \param theAllocator an allocator object
  GEOMALGOIMPL_EXPORT GEOMAlgo_Splitter(const Handle(NCollection_BaseAllocator)& theAllocator);
  
  GEOMALGOIMPL_EXPORT virtual ~GEOMAlgo_Splitter();

  /// Add a tool shape
  /// \param theShape a tool shape
  GEOMALGOIMPL_EXPORT void AddTool(const TopoDS_Shape& theShape);

  /// Returns list of tool shapes
  GEOMALGOIMPL_EXPORT const BOPCol_ListOfShape& Tools()const;

  /// Set type of used shapes
  /// \param aLimit a shape type
  GEOMALGOIMPL_EXPORT void SetLimit(const TopAbs_ShapeEnum aLimit);

  /// Returns defined limit type
  GEOMALGOIMPL_EXPORT TopAbs_ShapeEnum Limit()const;

  /// Set mode (0 or 1) of limit
  /// \param aMode the mode value
  GEOMALGOIMPL_EXPORT void SetLimitMode(const Standard_Integer aMode);

  /// Returns mode of limit
  GEOMALGOIMPL_EXPORT Standard_Integer LimitMode()const;

  /// Clears all tool shapes
  GEOMALGOIMPL_EXPORT virtual void Clear();

 protected:
   /// Build result.
   /// \param theType a type of limit
  GEOMALGOIMPL_EXPORT virtual void BuildResult(const TopAbs_ShapeEnum theType);

  /// Post processing of the calculation
  GEOMALGOIMPL_EXPORT virtual void PostTreat();
  
 protected:
   /// List of tools
  BOPCol_ListOfShape myTools; 

  /// Map of tools
  BOPCol_MapOfShape myMapTools;        

  /// A limit type
  TopAbs_ShapeEnum myLimit;   

  /// A limit mode
  Standard_Integer myLimitMode;  
};

#endif
