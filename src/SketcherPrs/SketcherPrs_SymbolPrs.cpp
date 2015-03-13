// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_SymbolPrs.cpp
// Created:     12 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_SymbolPrs.h"
#include "SketcherPrs_Tools.h"

#include <GeomAPI_Edge.h>

#include <Graphic3d_ArrayOfSegments.hxx>


#ifdef WIN32
# define FSEP "\\"
#else
# define FSEP "/"
#endif

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_SymbolPrs, AIS_InteractiveObject);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_SymbolPrs, AIS_InteractiveObject);


std::map<const char*, Handle(Image_AlienPixMap)> SketcherPrs_SymbolPrs::myIconsMap;


SketcherPrs_SymbolPrs::SketcherPrs_SymbolPrs(SketchPlugin_Constraint* theConstraint, 
                                             const std::shared_ptr<GeomAPI_Ax3>& thePlane)
 : AIS_InteractiveObject(), myConstraint(theConstraint), myPlane(thePlane)
{
  SetAutoHilight(Standard_False);
}


Handle(Image_AlienPixMap) SketcherPrs_SymbolPrs::icon()
{
  if (myIconsMap.count(iconName()) == 1) {
    return myIconsMap[iconName()];
  }
  TCollection_AsciiString aFile(getenv("NewGeomResources"));
  aFile += FSEP;
  aFile += iconName();
  Handle(Image_AlienPixMap) aPixMap = new Image_AlienPixMap();
  if (aPixMap->Load(aFile)) {
    myIconsMap[iconName()] = aPixMap;
    return aPixMap;
  }
  return Handle(Image_AlienPixMap)();
}

void SketcherPrs_SymbolPrs::ClearSelected()
{
  Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( NULL );  
  if( !aSelectionPrs.IsNull() ) {
    aSelectionPrs->Clear(); 
  }
}

void SketcherPrs_SymbolPrs::prepareAspect()
{
  if (myAspect.IsNull()) {
    myAspect = new Graphic3d_AspectMarker3d(icon());
  }
}

void SketcherPrs_SymbolPrs::addLine(const Handle(Graphic3d_Group)& theGroup, std::string theAttrName) const
{
  std::shared_ptr<GeomAPI_Shape> aLine = SketcherPrs_Tools::getLine(myConstraint, theAttrName);
  if (aLine.get() == NULL)
    return;
  std::shared_ptr<GeomAPI_Edge> aEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLine));

  std::shared_ptr<GeomAPI_Pnt> aPnt1 = aEdge->firstPoint();
  std::shared_ptr<GeomAPI_Pnt> aPnt2 = aEdge->lastPoint();

  Handle(Graphic3d_ArrayOfSegments) aLines = new Graphic3d_ArrayOfSegments(2, 1);
  aLines->AddVertex(aPnt1->impl<gp_Pnt>());
  aLines->AddVertex(aPnt2->impl<gp_Pnt>());
  theGroup->AddPrimitiveArray(aLines);
}
