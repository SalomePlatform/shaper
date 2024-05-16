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

// File:        GEOMAlgo_PassKeyMapHasher.cxx
// Created:
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_PassKeyShapeMapHasher.hxx>

#if OCC_VERSION_LARGE < 0x07080000

//=======================================================================
//function : HashCode
//purpose  :
//=======================================================================
  Standard_Integer GEOMAlgo_PassKeyShapeMapHasher::HashCode(const GEOMAlgo_PassKeyShape& aPK,
                                                            const Standard_Integer Upper)
{
  return aPK.HashCode(Upper);
}
//=======================================================================
//function :IsEqual
//purpose  :
//=======================================================================
  Standard_Boolean GEOMAlgo_PassKeyShapeMapHasher::IsEqual(const GEOMAlgo_PassKeyShape& aPK1,
                                                           const GEOMAlgo_PassKeyShape& aPK2)
{
  return aPK1.IsEqual(aPK2);
}

#else

size_t GEOMAlgo_PassKeyShapeMapHasher::operator()(const GEOMAlgo_PassKeyShape& aPKey) const
{
  return aPKey.GetSum();
}

bool GEOMAlgo_PassKeyShapeMapHasher::operator()(const GEOMAlgo_PassKeyShape& aPKey1,
                                                const GEOMAlgo_PassKeyShape& aPKey2) const
{
  return aPKey1.IsEqual(aPKey2);
}

#endif // OCC_VERSION_LARGE < 0x07080000
