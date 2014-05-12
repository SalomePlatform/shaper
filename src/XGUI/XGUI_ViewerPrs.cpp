// File:        XGUI_ViewerPrs.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_ViewerPrs.h"

XGUI_ViewerPrs::XGUI_ViewerPrs()
{
}

XGUI_ViewerPrs::XGUI_ViewerPrs(boost::shared_ptr<ModelAPI_Feature> theFeature,
                               const TopoDS_Shape& theShape)
: myFeature(theFeature), myShape(theShape)
{
}

XGUI_ViewerPrs::~XGUI_ViewerPrs()
{
}

void XGUI_ViewerPrs::setFeature(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  myFeature = theFeature;
}

void XGUI_ViewerPrs::setShape(const TopoDS_Shape& theShape)
{
  myShape = theShape;
}

boost::shared_ptr<ModelAPI_Feature> XGUI_ViewerPrs::feature() const
{
  return myFeature;
}

const TopoDS_Shape& XGUI_ViewerPrs::shape() const
{
  return myShape;
}
