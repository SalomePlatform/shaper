// File:        XGUI_ViewerPrs.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_ViewerPrs.h"

#include "SelectMgr_EntityOwner.hxx"

XGUI_ViewerPrs::XGUI_ViewerPrs()
{
}

XGUI_ViewerPrs::XGUI_ViewerPrs(FeaturePtr theFeature,
                               const TopoDS_Shape& theShape,
                               Handle(SelectMgr_EntityOwner) theOwner)
: myFeature(theFeature), myShape(theShape), myOwner(theOwner)
{
}

XGUI_ViewerPrs::~XGUI_ViewerPrs()
{
}

void XGUI_ViewerPrs::setFeature(FeaturePtr theFeature)
{
  myFeature = theFeature;
}

FeaturePtr XGUI_ViewerPrs::feature() const
{
  return myFeature;
}

void XGUI_ViewerPrs::setOwner(Handle(SelectMgr_EntityOwner) theOwner)
{
  myOwner = theOwner;
}

Handle(SelectMgr_EntityOwner) XGUI_ViewerPrs::owner() const
{
  return myOwner;
}

void XGUI_ViewerPrs::setShape(const TopoDS_Shape& theShape)
{
  myShape = theShape;
}

const TopoDS_Shape& XGUI_ViewerPrs::shape() const
{
  return myShape;
}
