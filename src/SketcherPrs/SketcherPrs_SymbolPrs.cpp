// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_SymbolPrs.cpp
// Created:     12 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_SymbolPrs.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Curve.h>

#include <Events_InfoMessage.h>

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_BndBox4f.hxx>

#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <TopLoc_Location.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <Prs3d_Root.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_Point.hxx>
#include <StdPrs_Curve.hxx>

#include <OpenGl_Element.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Group.hxx>

#ifdef WIN32
# define FSEP "\\"
#else
# define FSEP "/"
#endif

/// Step between icons
static const double MyDist = 0.02;


//**************************************************************
//! Redefinition of OpenGl_Element
class SketcherPrs_SymbolArray: public OpenGl_PrimitiveArray
{
public:
  SketcherPrs_SymbolArray(const OpenGl_GraphicDriver* theDriver,
    const Handle(SketcherPrs_SymbolPrs)& theObj, const Handle(AIS_InteractiveContext)& theCtx)
    :OpenGl_PrimitiveArray(theDriver, theObj->myPntArray->Type(), theObj->myPntArray->Indices(),
    theObj->myPntArray->Attributes(), theObj->myPntArray->Bounds()), myObj(theObj),
    myContext(theCtx) {}

  virtual void Render(const Handle(OpenGl_Workspace)& theWorkspace) const
  {
    ModelAPI_Feature* aConstraint = myObj->feature();
    if (aConstraint->data().get() && aConstraint->data()->isValid()) {
      Handle(OpenGl_View) aView = theWorkspace->View();
      double aScale = aView->Camera()->Scale();
      // Update points coordinate taking the viewer scale into account
      myObj->updateIfReadyToDisplay(MyDist * aScale);
      if (myIsVboInit) {
        if (myVboAttribs) {
          const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
          Handle(Graphic3d_Buffer) aAttr = myObj->myPntArray->Attributes();
          myVboAttribs->init(aCtx, 0, aAttr->NbElements,
                             aAttr->Data(), GL_NONE, aAttr->Stride);
        } else
          myIsVboInit = false;
      } else {
        myAttribs = myObj->myPntArray->Attributes();
        myIndices = myObj->myPntArray->Indices();
        myBounds = myObj->myPntArray->Bounds();
      }
    }
    OpenGl_PrimitiveArray::Render(theWorkspace);

    // Update selection position only if there is no selected object
    // because it can corrupt selection of other objects
    if ((myContext->NbCurrents() == 0) && (myContext->NbSelected() == 0))  {
      myContext->MainSelector()->RebuildSensitivesTree(myObj);
      myContext->MainSelector()->RebuildObjectsTree (false);
    }
  }

private:
  Handle(SketcherPrs_SymbolPrs) myObj;
  Handle(AIS_InteractiveContext) myContext;
};


//*****************************************************************************
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_SymbolPrs, AIS_InteractiveObject);


std::map<const char*, Handle(Image_AlienPixMap)> SketcherPrs_SymbolPrs::myIconsMap;


SketcherPrs_SymbolPrs::SketcherPrs_SymbolPrs(ModelAPI_Feature* theConstraint,
                                             const std::shared_ptr<GeomAPI_Ax3>& thePlane)
 : AIS_InteractiveObject(), myConstraint(theConstraint), myPlane(thePlane), myIsConflicting(false)
{
  SetAutoHilight(Standard_False);
  myPntArray = new Graphic3d_ArrayOfPoints(1);
  myPntArray->AddVertex(0., 0., 0.);
}

//*********************************************************************************
SketcherPrs_SymbolPrs::~SketcherPrs_SymbolPrs()
{
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  // Empty memory in position manager
  aMgr->deleteConstraint(this);
}

#ifdef _WINDOWS
#pragma warning( disable : 4996 )
#endif

//*********************************************************************************
Handle(Image_AlienPixMap) SketcherPrs_SymbolPrs::icon()
{
  if (myIconsMap.count(iconName()) == 1) {
    return myIconsMap[iconName()];
  }
  // Load icon for the presentation
  std::string aFile;
  char* anEnv = getenv("SHAPER_ROOT_DIR");
  if (anEnv) {
    aFile = std::string(anEnv) +
      FSEP + "share" + FSEP + "salome" + FSEP + "resources" + FSEP + "shaper";
  } else {
    anEnv = getenv("OPENPARTS_ROOT_DIR");
    if (anEnv)
      aFile = std::string(anEnv) + FSEP + "resources";
  }

  aFile += FSEP;
  aFile += iconName();
  Handle(Image_AlienPixMap) aPixMap = new Image_AlienPixMap();
  if (aPixMap->Load(aFile.c_str())) {
    myIconsMap[iconName()] = aPixMap;
    return aPixMap;
  }
  // The icon for constraint is not found
  static const char aMsg[] = "Error! constraint images are not found";
  cout<<aMsg<<endl;
  Events_InfoMessage("SketcherPrs_SymbolPrs", aMsg).send();
  myIconsMap[iconName()] = Handle(Image_AlienPixMap)();
  return Handle(Image_AlienPixMap)();
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::prepareAspect()
{
  // Create an aspect with the icon
  if (myAspect.IsNull()) {
    Handle(Image_AlienPixMap) aIcon = icon();
    if (aIcon.IsNull())
      myAspect = new Graphic3d_AspectMarker3d();
    else
      myAspect = new Graphic3d_AspectMarker3d(aIcon);
  }
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::addLine(const Handle(Graphic3d_Group)& theGroup,
                                    std::string theAttrName) const
{
  ObjectPtr aObj = SketcherPrs_Tools::getResult(myConstraint, theAttrName);
  std::shared_ptr<GeomAPI_Shape> aLine = SketcherPrs_Tools::getShape(aObj);
  if (aLine.get() == NULL)
    return;
  std::shared_ptr<GeomAPI_Edge> aEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLine));

  std::shared_ptr<GeomAPI_Pnt> aPnt1 = aEdge->firstPoint();
  std::shared_ptr<GeomAPI_Pnt> aPnt2 = aEdge->lastPoint();
  gp_Pnt aP1 = aPnt1->impl<gp_Pnt>();
  gp_Pnt aP2 = aPnt2->impl<gp_Pnt>();

  // Draw line by two points
  Handle(Graphic3d_ArrayOfSegments) aLines = new Graphic3d_ArrayOfSegments(2, 1);
  aLines->AddVertex(aPnt1->impl<gp_Pnt>());
  aLines->AddVertex(aPnt2->impl<gp_Pnt>());
  theGroup->AddPrimitiveArray(aLines);
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::HilightSelected(const Handle(PrsMgr_PresentationManager3d)& thePM,
                                            const SelectMgr_SequenceOfOwner& theOwners)
{

  Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( thePM );
  aSelectionPrs->Clear();
  drawLines(aSelectionPrs, GetContext()->SelectionStyle()->Color());

  aSelectionPrs->SetDisplayPriority(9);
  aSelectionPrs->Display();
  thePM->Color(this, GetContext()->SelectionStyle());
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::HilightOwnerWithColor(
                                  const Handle(PrsMgr_PresentationManager3d)& thePM,
                                  const Handle(Graphic3d_HighlightStyle)& theStyle,
                                  const Handle(SelectMgr_EntityOwner)& theOwner)
{
  thePM->Color(this, theStyle);

  Handle( Prs3d_Presentation ) aHilightPrs = GetHilightPresentation( thePM );
  aHilightPrs->Clear();
  drawLines(aHilightPrs, theStyle->Color());
  aHilightPrs->SetZLayer(Graphic3d_ZLayerId_Topmost);

  if (thePM->IsImmediateModeOn())
    thePM->AddToImmediateList(aHilightPrs);
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::Compute(
                const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                const Handle(Prs3d_Presentation)& thePresentation,
                const Standard_Integer theMode)
{
  // Create an icon
  prepareAspect();

  Handle(AIS_InteractiveContext) aCtx = GetContext();
  Handle(OpenGl_GraphicDriver) aDriver =
    Handle(OpenGl_GraphicDriver)::DownCast(aCtx->CurrentViewer()->Driver());
  if (aDriver.IsNull())
    return;

  // Update points with default shift value
  // it updates array of points if the presentation is ready to display, or the array of points
  // contains the previous values

  bool aReadyToDisplay = updateIfReadyToDisplay(20);

  int aNbVertex = myPntArray->VertexNumber();
  if (myOwner.IsNull()) {
    myOwner = new SelectMgr_EntityOwner(this);
  }

  // Create sensitive point for each symbol
  mySPoints.Clear();
  for (int i = 1; i <= aNbVertex; i++) {
    Handle(SketcherPrs_SensitivePoint) aSP = new SketcherPrs_SensitivePoint(myOwner, i);
    mySPoints.Append(aSP);
  }

  Handle(OpenGl_Group) aGroup = Handle(OpenGl_Group)::DownCast(thePresentation->NewGroup());
  aGroup->SetPrimitivesAspect(myAspect);

  // Recompute boundary box of the group
  Graphic3d_BndBox4f& aBnd = aGroup->ChangeBoundingBox();
  gp_Pnt aVert;
  aBnd.Clear();
  for (int i = 1; i <= myPntArray->ItemNumber(); i++) {
    aVert = myPntArray->Vertice(i);
    aBnd.Add(Graphic3d_Vec4((float)aVert.X(), (float)aVert.Y(), (float)aVert.Z(), 1.0f));
  }

  // Pint the group with custom procedure (see Render)
  SketcherPrs_SymbolArray* aElem =
    new SketcherPrs_SymbolArray((OpenGl_GraphicDriver*)aDriver->This(), this, GetContext());
  aGroup->AddElement(aElem);

  // Disable frustum culling for this object by marking it as mutable
  aGroup->Structure()->SetMutable(true);

  if (!aReadyToDisplay)
    SketcherPrs_Tools::sendEmptyPresentationError(myConstraint,
                          "An empty AIS presentation: SketcherPrs_LengthDimension");
}


//*********************************************************************************
void SketcherPrs_SymbolPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                             const Standard_Integer aMode)
{
  ClearSelected();
  if ((aMode == 0) || (aMode == SketcherPrs_Tools::Sel_Constraint)) {
    for (int i = 1; i <= mySPoints.Length(); i++)
      aSelection->Add(mySPoints.Value(i));
  }
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::SetConflictingConstraint(const bool& theConflicting,
                                                     const std::vector<int>& theColor)
{
  if (theConflicting)
  {
    if (!myAspect.IsNull())
      myAspect->SetColor (Quantity_Color (theColor[0] / 255., theColor[1] / 255.,
                          theColor[2] / 255., Quantity_TOC_RGB));
    myIsConflicting = true;
  }
  else
  {
    if (!myAspect.IsNull())
      myAspect->SetColor (Quantity_Color (1.0, 1.0, 0.0, Quantity_TOC_RGB));
    myIsConflicting = false;
  }
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::drawShape(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                      const Handle(Prs3d_Presentation)& thePrs) const
{
  if (theShape->isEdge()) {
    // The shape is edge
    std::shared_ptr<GeomAPI_Curve> aCurve =
      std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(theShape));
    if (aCurve->isLine()) {
      // The shape is line
      GeomAdaptor_Curve
        aCurv(aCurve->impl<Handle(Geom_Curve)>(), aCurve->startParam(), aCurve->endParam());
      StdPrs_Curve::Add(thePrs, aCurv, myDrawer);
    } else {
      // The shape is circle or arc
      GeomAdaptor_Curve
        aAdaptor(aCurve->impl<Handle(Geom_Curve)>(), aCurve->startParam(), aCurve->endParam());
      StdPrs_DeflectionCurve::Add(thePrs,aAdaptor,myDrawer);
    }
  } else if (theShape->isVertex()) {
    // draw vertex
    std::shared_ptr<GeomAPI_Vertex> aVertex =
      std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(theShape));
    std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
    Handle(Geom_CartesianPoint) aPoint = new Geom_CartesianPoint(aPnt->impl<gp_Pnt>());
    StdPrs_Point::Add(thePrs, aPoint, myDrawer);
  }
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::drawListOfShapes(
                      const std::shared_ptr<ModelAPI_AttributeRefList>& theListAttr,
                      const Handle(Prs3d_Presentation)& thePrs) const
{
  int aNb = theListAttr->size();
  if (aNb == 0)
    return;
  int i;
  ObjectPtr aObj;
  for (i = 0; i < aNb; i++) {
    aObj = theListAttr->object(i);
    std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(aObj);
    if (aShape.get() != NULL)
      drawShape(aShape, thePrs);
  }
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::BoundingBox(Bnd_Box& theBndBox)
{
  Select3D_BndBox3d aTmpBox;
  for (Select3D_EntitySequenceIter aPntIter (mySPoints); aPntIter.More(); aPntIter.Next()) {
    const Handle(Select3D_SensitiveEntity)& anEnt = aPntIter.Value();
    aTmpBox.Combine (anEnt->BoundingBox());
  }

  theBndBox.Update (aTmpBox.CornerMin().x(), aTmpBox.CornerMin().y(), aTmpBox.CornerMin().z(),
                    aTmpBox.CornerMax().x(), aTmpBox.CornerMax().y(), aTmpBox.CornerMax().z());
}

