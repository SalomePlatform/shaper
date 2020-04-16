// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketcherPrs_SymbolPrs.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Curve.h>

#include <ModelAPI_Tools.h>

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
#include <Prs3d_LineAspect.hxx>

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
static const double MyDist = 0.015;


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
      myObj->updateIfReadyToDisplay(MyDist * aScale, myObj->myIsCustomColor);
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
    if (myContext->NbSelected() == 0)  {
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
  SketchPlugin_Sketch* theSketcher)
 : AIS_InteractiveObject(), myConstraint(theConstraint),
   myIsCustomColor(false),
   mySketcher(theSketcher)
{
  SetAutoHilight(Standard_False);
}

//*********************************************************************************
SketcherPrs_SymbolPrs::~SketcherPrs_SymbolPrs()
{
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  // Empty memory in position manager
  aMgr->deleteConstraint(this);

  Handle(Prs3d_Presentation) aSelPrs =
    GetSelectPresentation(Handle(PrsMgr_PresentationManager3d)());
  if (!aSelPrs.IsNull()) {
      if (!aSelPrs->Groups().IsEmpty()) {
        aSelPrs->Clear();
      }
      aSelPrs->Erase();
  }
  Handle(Prs3d_Presentation) aHilightPrs =
    GetHilightPresentation(Handle(PrsMgr_PresentationManager3d)());
  if (!aHilightPrs.IsNull()) {
    if (!aHilightPrs->Groups().IsEmpty()) {
      aHilightPrs->Clear();
    }
    aHilightPrs->Erase();
  }
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
    anEnv = getenv("CADBUILDER_ROOT_DIR");
    if (anEnv)
      aFile = std::string(anEnv) + FSEP + "resources";
  }

  aFile += FSEP;
  aFile += iconName();
  Handle(Image_AlienPixMap) aPixMap = new Image_AlienPixMap();
  if (aPixMap->Load(aFile.c_str())) {
    int aRatio = SketcherPrs_Tools::pixelRatio();
    if (aRatio > 1) {
      Handle(Image_AlienPixMap) aSizedMap = new Image_AlienPixMap();
      Standard_Size aWidth = aPixMap->Width() * aRatio;
      Standard_Size aHeigh = aPixMap->Height() * aRatio;
      aSizedMap->InitTrash(aPixMap->Format(), aWidth, aHeigh);
      for (Standard_Size i = 0; i < aWidth; i++) {
        for (Standard_Size j = 0; j < aHeigh; j++) {
          aSizedMap->SetPixelColor(int(i), int(j), aPixMap->PixelColor(i / aRatio, j / aRatio));
        }
      }
      aPixMap = aSizedMap;
    }
    myIconsMap[iconName()] = aPixMap;
    return aPixMap;
  }
  // The icon for constraint is not found
  static const char aMsg[] = "Error! constraint images are not found";
  std::cout<<aMsg<<std::endl;
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

    myAspect->SetColor(myCustomColor);
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
                                  const Handle(Prs3d_Drawer)& theStyle,
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
  if (!plane().get())
    return;
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

  bool aReadyToDisplay = updateIfReadyToDisplay(20, myIsCustomColor);

  int aNbVertex = myPntArray->VertexNumber();
  if (myOwner.IsNull()) {
    myOwner = new SelectMgr_EntityOwner(this);
  }

  // Create sensitive point for each symbol
  mySPoints.Clear();
  for (int i = 1; i <= aNbVertex; i++) {
    Handle(SketcherPrs_SensitivePoint) aSP = new SketcherPrs_SensitivePoint(myOwner, i);
    mySPoints.Append(aSP);
    if (myIsCustomColor)
      myPntArray->SetVertexColor(i, myCustomColor);
  }

  Handle(OpenGl_Group) aGroup =
    Handle(OpenGl_Group)::DownCast(Prs3d_Root::CurrentGroup (thePresentation));
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
void SketcherPrs_SymbolPrs::SetCustomColor(const std::vector<int>& theColor)
{
  myIsCustomColor = !theColor.empty();
  if (myIsCustomColor)
    myCustomColor = Quantity_Color(theColor[0] / 255., theColor[1] / 255.,
                                   theColor[2] / 255., Quantity_TOC_RGB);
  else
    myCustomColor = Quantity_Color (1.0, 1.0, 0.0, Quantity_TOC_RGB);

  if (!myAspect.IsNull())
    myAspect->SetColor (myCustomColor);

  Handle(Prs3d_Presentation) aPrs = Presentation();
  if (!aPrs.IsNull()) {
    if (myIsCustomColor) {
      Handle(Graphic3d_PresentationAttributes) aAttr = new Graphic3d_PresentationAttributes();
      aAttr->SetColor(myCustomColor);
      aPrs->Highlight(aAttr);
    }
    else {
      aPrs->UnHighlight();
    }
  }
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::drawShape(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                      const Handle(Prs3d_Presentation)& thePrs,
                                      Quantity_Color theColor) const
{
  Handle(Graphic3d_AspectLine3d) aLineAspect =
    new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);

  Handle(Prs3d_LineAspect) aLinesStyle = myDrawer->LineAspect();
  Handle(Graphic3d_AspectLine3d) aOldStyle = aLinesStyle->Aspect();
  aLinesStyle->SetAspect(aLineAspect);
  myDrawer->SetLineAspect(aLinesStyle);

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

  aLinesStyle->SetAspect(aOldStyle);
  myDrawer->SetLineAspect(aLinesStyle);
}

//*********************************************************************************
void SketcherPrs_SymbolPrs::drawListOfShapes(
                      const std::shared_ptr<ModelAPI_AttributeRefList>& theListAttr,
                      const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
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
      drawShape(aShape, thePrs, theColor);
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
