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

#include<GeomAPI_Trsf.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Ax2.h>

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Trsf.hxx>

#define MY_TRSF implPtr<gp_Trsf>()

#include <iostream>

//=================================================================================================
GeomAPI_Trsf::GeomAPI_Trsf()
: GeomAPI_Interface(new gp_Trsf())
{
}

//=================================================================================================
GeomAPI_Trsf::GeomAPI_Trsf(void* theTrsf)
: GeomAPI_Interface((gp_Trsf*)theTrsf)
{
}

//=================================================================================================
void GeomAPI_Trsf::setTranslation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                  const double theDistance)
{
  MY_TRSF->SetTranslation(gp_Vec(theAxis->impl<gp_Ax1>().Direction()) * theDistance);
}

//=================================================================================================
void GeomAPI_Trsf::setTranslation(const double theDx, const double theDy, const double theDz)
{
  MY_TRSF->SetTranslation(gp_Vec(theDx, theDy, theDz));
}

//=================================================================================================
void GeomAPI_Trsf::setTranslation(const std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                                  const std::shared_ptr<GeomAPI_Pnt> theEndPoint)
{
  MY_TRSF->SetTranslation(theStartPoint->impl<gp_Pnt>(), theEndPoint->impl<gp_Pnt>());
}

//=================================================================================================
void GeomAPI_Trsf::setRotation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                               const double theAngle)
{
  MY_TRSF->SetRotation(theAxis->impl<gp_Ax1>(), theAngle / 180.0 * M_PI);
}

//=================================================================================================
void GeomAPI_Trsf::setRotation(const std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                               const std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                               const std::shared_ptr<GeomAPI_Pnt> theEndPoint)
{
  gp_Pnt aCenterPoint = theCenterPoint->impl<gp_Pnt>();
  gp_Pnt aStartPoint = theStartPoint->impl<gp_Pnt>();
  gp_Pnt aEndPoint = theEndPoint->impl<gp_Pnt>();

  gp_Vec aVec1(aCenterPoint, aStartPoint);
  gp_Vec aVec2(aCenterPoint, aEndPoint);
  gp_Dir aDir(aVec1 ^ aVec2);
  gp_Ax1 anAxis(aCenterPoint, aDir);
  double anAngle = aVec1.Angle(aVec2);
  if (fabs(anAngle) < Precision::Angular()) anAngle += 2.*M_PI; // Taken from old GEOM code

  MY_TRSF->SetRotation(anAxis, anAngle);
}

//=================================================================================================
void GeomAPI_Trsf::setSymmetry(const std::shared_ptr<GeomAPI_Pnt> thePoint)
{
  MY_TRSF->SetMirror(thePoint->impl<gp_Pnt>());
}

//=================================================================================================
void GeomAPI_Trsf::setSymmetry(const std::shared_ptr<GeomAPI_Ax1> theAxis)
{
  MY_TRSF->SetMirror(theAxis->impl<gp_Ax1>());
}

//=================================================================================================
void GeomAPI_Trsf::setSymmetry(const std::shared_ptr<GeomAPI_Ax2> thePlane)
{
  MY_TRSF->SetMirror(thePlane->impl<gp_Ax2>());
}

//=================================================================================================
void GeomAPI_Trsf::setScale(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                            const double theScale)
{
  MY_TRSF->SetScale(theCenter->impl<gp_Pnt>(), theScale);
}
