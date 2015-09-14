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

#include <Events_Error.h>

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
#include <OpenGl_PointSprite.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_ShaderManager.hxx>

#ifdef WIN32
# define FSEP "\\"
#else
# define FSEP "/"
#endif

/// Step between icons
static const double MyDist = 0.02;

/// Function to convert opengl data type
GLenum toGlDataType (const Graphic3d_TypeOfData theType, GLint& theNbComp)
{
  switch (theType) {
    case Graphic3d_TOD_USHORT:
      theNbComp = 1;
      return GL_UNSIGNED_SHORT;
    case Graphic3d_TOD_UINT:
      theNbComp = 1;
      return GL_UNSIGNED_INT;
    case Graphic3d_TOD_VEC2:
      theNbComp = 2;
      return GL_FLOAT;
    case Graphic3d_TOD_VEC3:
      theNbComp = 3;
      return GL_FLOAT;
    case Graphic3d_TOD_VEC4:
      theNbComp = 4;
      return GL_FLOAT;
    case Graphic3d_TOD_VEC4UB:
      theNbComp = 4;
      return GL_UNSIGNED_BYTE;
  }
  theNbComp = 0;
  return GL_NONE;
}


//*******************************************************************
//! Auxiliary class for Vertex buffer with interleaved attributes.
class SketcherPrs_VertexBuffer : public OpenGl_VertexBuffer
{

public:

  //! Create uninitialized VBO..
  SketcherPrs_VertexBuffer (const Graphic3d_Attribute* theAttribs,
                        const Standard_Integer     theStride)
  : Stride (theStride), NbAttributes(1)
  {

    memcpy (Attribs, theAttribs, sizeof(Graphic3d_Attribute) * NbAttributes);
  }

  //! Create uninitialized VBO.
  SketcherPrs_VertexBuffer (const Graphic3d_Buffer& theAttribs)
  : Stride (theAttribs.Stride), NbAttributes(1)
  {
    memcpy (Attribs, theAttribs.AttributesArray(), sizeof(Graphic3d_Attribute) * NbAttributes);
  }

  virtual bool HasColorAttribute() const
  {
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter) {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      if (anAttrib.Id == Graphic3d_TOA_COLOR) {
        return true;
      }
    }
    return false;
  }

  virtual bool HasNormalAttribute() const
  {
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter) {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      if (anAttrib.Id == Graphic3d_TOA_NORM) {
        return true;
      }
    }
    return false;
  }

  virtual void BindPositionAttribute (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!OpenGl_VertexBuffer::IsValid()) {
      return;
    }

    OpenGl_VertexBuffer::Bind (theGlCtx);
    GLint aNbComp;
    const GLubyte* anOffset = OpenGl_VertexBuffer::myOffset;
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter) {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      const GLenum   aDataType = toGlDataType (anAttrib.DataType, aNbComp);
      if (aDataType == GL_NONE) {
        continue;
      } else if (anAttrib.Id == Graphic3d_TOA_POS) {
        OpenGl_VertexBuffer::bindAttribute (theGlCtx, Graphic3d_TOA_POS, aNbComp, aDataType, Stride, anOffset);
        break;
      }

      anOffset += Graphic3d_Attribute::Stride (anAttrib.DataType);
    }
  }

  virtual void BindAllAttributes (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!OpenGl_VertexBuffer::IsValid())
      return;

    OpenGl_VertexBuffer::Bind (theGlCtx);
    GLint aNbComp;
    const GLubyte* anOffset = OpenGl_VertexBuffer::myOffset;
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      const GLenum   aDataType = toGlDataType (anAttrib.DataType, aNbComp);
      if (aDataType == GL_NONE)
        continue;

      OpenGl_VertexBuffer::bindAttribute (theGlCtx, anAttrib.Id, aNbComp, aDataType, Stride, anOffset);
      anOffset += Graphic3d_Attribute::Stride (anAttrib.DataType);
    }
  }

  virtual void UnbindAllAttributes (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!OpenGl_VertexBuffer::IsValid())
      return;
    OpenGl_VertexBuffer::Unbind (theGlCtx);

    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter) {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      OpenGl_VertexBuffer::unbindAttribute (theGlCtx, anAttrib.Id);
    }
  }

public:

  Graphic3d_Attribute Attribs[1];
  Standard_Integer    Stride;
  Standard_Integer NbAttributes;
};

//**************************************************************
//! Redefinition of OpenGl_Element
class SketcherPrs_Element: public OpenGl_Element
{
public:
  SketcherPrs_Element(const Handle(SketcherPrs_SymbolPrs)& theObj) : 
  OpenGl_Element(), myObj(theObj) {}

  virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const
  {
    if (!myObj.IsNull())
      myObj->Render(theWorkspace);
  }

  virtual void Release (OpenGl_Context* theContext) 
  {
    if (!myObj.IsNull())
      myObj->Release(theContext);
  }

private:
  Handle(SketcherPrs_SymbolPrs) myObj;
};


//**************************************************************
//! Definition of call back
OpenGl_Element* SymbolPrsCallBack(const CALL_DEF_USERDRAW * theUserDraw)
{
  Handle(SketcherPrs_SymbolPrs) anIObj = (SketcherPrs_SymbolPrs*)theUserDraw->Data;
  if (anIObj.IsNull()) {
    std::cout << "VUserDrawCallback error: null object passed, the custom scene element will not be rendered" << std::endl;
  }
  return new SketcherPrs_Element(anIObj);
}


//*****************************************************************************
IMPLEMENT_STANDARD_HANDLE(SketcherPrs_SymbolPrs, AIS_InteractiveObject);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_SymbolPrs, AIS_InteractiveObject);


std::map<const char*, Handle(Image_AlienPixMap)> SketcherPrs_SymbolPrs::myIconsMap;


SketcherPrs_SymbolPrs::SketcherPrs_SymbolPrs(ModelAPI_Feature* theConstraint, 
                                             const std::shared_ptr<GeomAPI_Ax3>& thePlane)
 : AIS_InteractiveObject(), myConstraint(theConstraint), myPlane(thePlane)
{
  SetAutoHilight(Standard_False);
}

SketcherPrs_SymbolPrs::~SketcherPrs_SymbolPrs()
{
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  aMgr->deleteConstraint(this);
}

#ifdef _WINDOWS
#pragma warning( disable : 4996 )
#endif

Handle(Image_AlienPixMap) SketcherPrs_SymbolPrs::icon()
{
  if (myIconsMap.count(iconName()) == 1) {
    return myIconsMap[iconName()];
  }
  char* aEnv = getenv("NEWGEOM_ROOT_DIR");
  if (aEnv != NULL) {
    TCollection_AsciiString aFile(aEnv);
    aFile+=FSEP;
    aFile+="resources";
    aFile += FSEP;
    aFile += iconName();
    Handle(Image_AlienPixMap) aPixMap = new Image_AlienPixMap();
    if (aPixMap->Load(aFile)) {
      myIconsMap[iconName()] = aPixMap;
      return aPixMap;
    }
  }
  static const char aMsg[] = "Error! constraint images are not found";
  cout<<aMsg<<endl;
  Events_Error::send(aMsg);
  myIconsMap[iconName()] = Handle(Image_AlienPixMap)();
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
    Handle(Image_AlienPixMap) aIcon = icon();
    if (aIcon.IsNull()) 
      myAspect = new Graphic3d_AspectMarker3d();
    else
      myAspect = new Graphic3d_AspectMarker3d(aIcon);
  }
}

void SketcherPrs_SymbolPrs::addLine(const Handle(Graphic3d_Group)& theGroup, std::string theAttrName) const
{
  ObjectPtr aObj = SketcherPrs_Tools::getResult(myConstraint, theAttrName);
  std::shared_ptr<GeomAPI_Shape> aLine = SketcherPrs_Tools::getShape(aObj);
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

void SketcherPrs_SymbolPrs::HilightSelected(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                           const SelectMgr_SequenceOfOwner& theOwners)
{

  Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( thePM );
  aSelectionPrs->Clear();
  drawLines(aSelectionPrs, Quantity_NOC_WHITE);

  aSelectionPrs->SetDisplayPriority(9);
  aSelectionPrs->Display();
  thePM->Highlight(this);
}

void SketcherPrs_SymbolPrs::HilightOwnerWithColor(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                                 const Quantity_NameOfColor theColor, 
                                                 const Handle(SelectMgr_EntityOwner)& theOwner)
{
  thePM->Color(this, theColor);

  Handle( Prs3d_Presentation ) aHilightPrs = GetHilightPresentation( thePM );
  aHilightPrs->Clear();
  drawLines(aHilightPrs, theColor);

  if (thePM->IsImmediateModeOn())
    thePM->AddToImmediateList(aHilightPrs);
}

void SketcherPrs_SymbolPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  prepareAspect();

  Handle(AIS_InteractiveContext) aCtx = GetContext();
  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(aCtx->CurrentViewer()->Driver());
  if (!aDriver.IsNull()) {
    // register the custom element factory function
    aDriver->UserDrawCallback() = SymbolPrsCallBack;
  }

  if (!updatePoints(20)) {
    return;
  }

  int aNbVertex = myPntArray->VertexNumber();
  if (myOwner.IsNull()) {
    myOwner = new SelectMgr_EntityOwner(this);
  }

  mySPoints.Clear();
  for (int i = 1; i <= aNbVertex; i++) {
    Handle(SketcherPrs_SensitivePoint) aSP = new SketcherPrs_SensitivePoint(myOwner, i);
    mySPoints.Append(aSP);
  }

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePresentation);
  aGroup->SetPrimitivesAspect(myAspect);

  Graphic3d_BndBox4f& aBnd = aGroup->ChangeBoundingBox();
  gp_Pnt aVert;
  aBnd.Clear();
  for (int i = 1; i <= myPntArray->ItemNumber(); i++) {
    aVert = myPntArray->Vertice(i);
    aBnd.Add (Graphic3d_Vec4((float)aVert.X(), (float)aVert.Y(), (float)aVert.Z(), 1.0f));
  }

  aGroup->UserDraw(this, true);

  // Disable frustum culling for this object by marking it as mutable
  aGroup->Structure()->SetMutable(true);
  //aGroup->AddPrimitiveArray(myPntArray);
}



void SketcherPrs_SymbolPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  ClearSelected();
  if ((aMode == 0) || (aMode == SketcherPrs_Tools::Sel_Constraint)) {
    for (int i = 1; i <= mySPoints.Length(); i++)
      aSelection->Add(mySPoints.Value(i));
  }
}


void SketcherPrs_SymbolPrs::Render(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // do not update presentation for invalid or already removed objects: the presentation
  // should be removed soon
  if (!myConstraint->data().get() || !myConstraint->data()->isValid())
    return;

  const OpenGl_AspectMarker* anAspectMarker = theWorkspace->AspectMarker(Standard_True);
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  Handle(OpenGl_View) aView = theWorkspace->ActiveView();
  
  double aScale = aView->Camera()->Scale();
  if (!updatePoints(MyDist * aScale))
    return;

  Handle(Graphic3d_Buffer) aAttribs = myPntArray->Attributes();

  if (myVboAttribs.IsNull()) {
    myVboAttribs = new SketcherPrs_VertexBuffer(*aAttribs);
  }

  if (!myVboAttribs->init(aCtx, 0, aAttribs->NbElements, aAttribs->Data(), GL_NONE, aAttribs->Stride)) {
    myVboAttribs->Release (aCtx.operator->());
    myVboAttribs.Nullify();
    return;
  }
    
  Handle(OpenGl_Texture) aTextureBack = theWorkspace->DisableTexture();

  const Handle(OpenGl_PointSprite)& aSpriteNorm = anAspectMarker->SpriteRes(aCtx);
      
  if (!aSpriteNorm.IsNull() && !aSpriteNorm->IsDisplayList()) {
    const bool toHilight = (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT) != 0;
    const Handle(OpenGl_PointSprite)& aSprite = (toHilight && anAspectMarker->SpriteHighlightRes(aCtx)->IsValid())
                                              ? anAspectMarker->SpriteHighlightRes(aCtx)
                                              : aSpriteNorm;
    theWorkspace->EnableTexture (aSprite);
    aCtx->ShaderManager()->BindProgram(anAspectMarker, aSprite, Standard_False, Standard_False, anAspectMarker->ShaderProgramRes(aCtx));
    const TEL_COLOUR* aLineColor  =  &anAspectMarker->Color();
    if (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)
      aLineColor = theWorkspace->HighlightColor;

    if (toHilight)
      aCtx->core11fwd->glDisable (GL_LIGHTING);
    else
      aCtx->core11fwd->glEnable (GL_LIGHTING);

    aCtx->SetColor4fv(*(const OpenGl_Vec4* )(aLineColor->rgb));


    myVboAttribs->BindAllAttributes(aCtx);
    // Textured markers will be drawn with the point sprites
    aCtx->SetPointSize (anAspectMarker->MarkerSize());
    aCtx->core11fwd->glEnable (GL_ALPHA_TEST);
    aCtx->core11fwd->glAlphaFunc (GL_GEQUAL, 0.1f);

    aCtx->core11fwd->glEnable (GL_BLEND);
    aCtx->core11fwd->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    aCtx->core11fwd->glDrawArrays (0, 0, myVboAttribs->GetElemsNb());

    aCtx->core11fwd->glDisable (GL_BLEND);
    aCtx->core11fwd->glDisable (GL_ALPHA_TEST);
    aCtx->SetPointSize (1.0f);
  }
  theWorkspace->EnableTexture (aTextureBack);
  aCtx->BindProgram (NULL);

  // Update selection position only if there is no selected object
  // because it can corrupt selection of other objects
  if ((GetContext()->NbCurrents() == 0) && (GetContext()->NbSelected() == 0))
  {
    GetContext()->MainSelector()->RebuildSensitivesTree (this);
    GetContext()->MainSelector()->RebuildObjectsTree (false);
  }
}


void SketcherPrs_SymbolPrs::Release (OpenGl_Context* theContext)
{
  if (!myVboAttribs.IsNull()) {
    if (theContext) {
      theContext->DelayedRelease (myVboAttribs);
    }
    myVboAttribs.Nullify();
  }
}

void SketcherPrs_SymbolPrs::drawShape(const std::shared_ptr<GeomAPI_Shape>& theShape, 
                                      const Handle(Prs3d_Presentation)& thePrs) const
{
  if (theShape->isEdge()) {
    std::shared_ptr<GeomAPI_Curve> aCurve = 
      std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(theShape));
    if (aCurve->isLine()) {
      GeomAdaptor_Curve aCurv(aCurve->impl<Handle(Geom_Curve)>(), aCurve->startParam(), aCurve->endParam());
      StdPrs_Curve::Add(thePrs, aCurv, myDrawer);
    } else {
      GeomAdaptor_Curve aAdaptor(aCurve->impl<Handle(Geom_Curve)>(), aCurve->startParam(), aCurve->endParam());
      StdPrs_DeflectionCurve::Add(thePrs,aAdaptor,myDrawer);
    }
  } else if (theShape->isVertex()) {
    std::shared_ptr<GeomAPI_Vertex> aVertex = 
      std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(theShape));
    std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
    Handle(Geom_CartesianPoint) aPoint = new Geom_CartesianPoint(aPnt->impl<gp_Pnt>());
    StdPrs_Point::Add(thePrs, aPoint, myDrawer);
  }
}

void SketcherPrs_SymbolPrs::drawListOfShapes(const std::shared_ptr<ModelAPI_AttributeRefList>& theListAttr, 
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

void SketcherPrs_SymbolPrs::BoundingBox (Bnd_Box& theBndBox)
{
  Select3D_BndBox3d aTmpBox;
  for (Select3D_EntitySequenceIter aPntIter (mySPoints); aPntIter.More(); aPntIter.Next())
  {
    const Handle(Select3D_SensitiveEntity)& anEnt = aPntIter.Value();
    aTmpBox.Combine (anEnt->BoundingBox());
  }

  theBndBox.Update (aTmpBox.CornerMin().x(), aTmpBox.CornerMin().y(), aTmpBox.CornerMin().z(),
                    aTmpBox.CornerMax().x(), aTmpBox.CornerMax().y(), aTmpBox.CornerMax().z());
}

