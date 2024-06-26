// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// File:        GEOMAlgo_PassKeyMapHasher.hxx
// Created:
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#ifndef _GEOMAlgo_PassKeyShapeMapHasher_HeaderFile
#define _GEOMAlgo_PassKeyShapeMapHasher_HeaderFile

#include <GeomAlgoImpl.h>

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <GEOMAlgo_PassKeyShape.hxx>

#include <Basics_OCCTVersion.hxx>

//=======================================================================
//class    : GEOMAlgo_PassKeyShapeMapHasher
//purpose  :
//=======================================================================
class GEOMAlgo_PassKeyShapeMapHasher
{
 public:
#if OCC_VERSION_LARGE < 0x07080000
  GEOMALGOIMPL_EXPORT
    static  Standard_Integer HashCode(const GEOMAlgo_PassKeyShape& aPKey,
                                      const Standard_Integer Upper) ;

  GEOMALGOIMPL_EXPORT
    static  Standard_Boolean IsEqual(const GEOMAlgo_PassKeyShape& aPKey1,
                                     const GEOMAlgo_PassKeyShape& aPKey2) ;
#else
  GEOMALGOIMPL_EXPORT
  size_t operator()(const GEOMAlgo_PassKeyShape& aPKey) const;

  GEOMALGOIMPL_EXPORT
  bool operator()(const GEOMAlgo_PassKeyShape& aPKey1,
                  const GEOMAlgo_PassKeyShape& aPKey2) const;
#endif // OCC_VERSION_LARGE < 0x07080000
};
#endif
