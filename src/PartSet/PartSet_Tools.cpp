// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Tools.h>

#include <V3d_View.hxx>
#include <gp_Pln.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

gp_Pnt PartSet_Tools::ConvertClickToPoint(QPoint thePoint, Handle(V3d_View) theView)
{
  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  theView->Eye( XEye, YEye, ZEye );

  theView->At( XAt, YAt, ZAt );
  gp_Pnt EyePoint( XEye, YEye, ZEye );
  gp_Pnt AtPoint( XAt, YAt, ZAt );

  gp_Vec EyeVector( EyePoint, AtPoint );
  gp_Dir EyeDir( EyeVector );

  gp_Pln PlaneOfTheView = gp_Pln( AtPoint, EyeDir );
  Standard_Real X, Y, Z;
  theView->Convert( thePoint.x(), thePoint.y(), X, Y, Z );
  gp_Pnt ConvertedPoint( X, Y, Z );

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project( PlaneOfTheView, ConvertedPoint );
  gp_Pnt ResultPoint = ElSLib::Value( ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(), PlaneOfTheView );
  return ResultPoint;
}
