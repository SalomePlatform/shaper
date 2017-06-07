// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModuleBase_ViewerFilters.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IModule.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Edge.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#ifdef BEFORE_TRIHEDRON_PATCH
#include <AIS_Axis.hxx>
#include <AIS_Point.hxx>
#include <AIS_Plane.hxx>
#else
#include <AIS_Trihedron.hxx>
#include <AIS_TrihedronOwner.hxx>
#include <Geom_Axis2Placement.hxx>
#endif
#include <Geom_Point.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>

#include <StdSelect_BRepOwner.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pln.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);


//TODO (VSV): Check bug in OCCT: Filter result is ignored (bug25340)
Standard_Boolean ModuleBase_ShapeDocumentFilter::IsOk(
                                            const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  bool aValid = true;

  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  // the shapes from different documents should be provided if there is no started operation
  // in order to show/hide results
  if (anOperation) {
    aValid = Standard_False;
    std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
    if (theOwner->HasSelectable()) {
      Handle(AIS_InteractiveObject) aAisObj =
                       Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
      if (!aAisObj.IsNull()) {
        aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
      }
    }
    ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);
    if (aObj) {
      DocumentPtr aDoc = aObj->document();
      SessionPtr aMgr = ModelAPI_Session::get();
      aValid = (aDoc == aMgr->activeDocument() || aDoc == aMgr->moduleDocument());
    }
    else {
      // This object is not controlled by the filter
      aValid = Standard_True;
    }
  }

#ifdef DEBUG_FILTERS
  qDebug(QString("ModuleBase_ShapeDocumentFilter::IsOk = %1").arg(aValid).toStdString().c_str());
#endif
  return aValid;
}

IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);

Standard_Boolean ModuleBase_ShapeInPlaneFilter::IsOk(
                                        const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  bool aValid = true;

  if (myPlane.get()) {
    aValid = Standard_False;
    // selectable may be empty if the object is selected in Object Browser and is not visualized
    // in the viewer(virtual owner is created for this case)
    //if (theOwner->HasSelectable())
    {
      gp_Pln aPlane = myPlane->impl<gp_Pln>();
      Handle(StdSelect_BRepOwner) aShapeOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
      if (!aShapeOwner.IsNull() && aShapeOwner->HasShape()) {
        TopoDS_Shape aShape = aShapeOwner->Shape();
        TopAbs_ShapeEnum aType = aShape.ShapeType();
        switch (aType) {
        case TopAbs_VERTEX:
          {
            gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
            return aPlane.SquareDistance(aPnt) < Precision::SquareConfusion();
          }
        case TopAbs_EDGE:
          {
            std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge);
            anEdge->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
            return anEdge->isInPlane(myPlane);
          }
        default:
          // The object can be selected in Object browser and contain, for example, compound.
          // The compound could not belong to any plane, so the result is false
          aValid = Standard_False;
        break;
        }
      } else {
        if (theOwner->HasSelectable()) {
        // Check Trihedron sub-objects
        Handle(SelectMgr_SelectableObject) aSelObj = theOwner->Selectable();
        Handle(Standard_Type) aType = aSelObj->DynamicType();
#ifdef BEFORE_TRIHEDRON_PATCH
        if (aType == STANDARD_TYPE(AIS_Axis)) {
          Handle(AIS_Axis) aAxis = Handle(AIS_Axis)::DownCast(aSelObj);
          gp_Lin aLine = aAxis->Component()->Lin();
          return aPlane.Contains(aLine, Precision::Confusion(), Precision::Angular());

        } else if (aType == STANDARD_TYPE(AIS_Point)) {
          Handle(AIS_Point) aPoint = Handle(AIS_Point)::DownCast(aSelObj);
          return aPlane.Distance(aPoint->Component()->Pnt()) < Precision::Confusion();

        } else if (aType == STANDARD_TYPE(AIS_Plane)) {
          Handle(AIS_Plane) aAisPlane = Handle(AIS_Plane)::DownCast(aSelObj);
          gp_Pln aPln = aAisPlane->Component()->Pln();
          return aPlane.Distance(aPln) < Precision::Confusion();
        }
#else
        if (aType == STANDARD_TYPE(AIS_Trihedron)) {
          Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast(aSelObj);
          Handle(AIS_TrihedronOwner) aTrOwner = Handle(AIS_TrihedronOwner)::DownCast(theOwner);
          if (!aTrOwner.IsNull())
          {
            const Prs3d_DatumParts& aPart = aTrOwner->DatumPart();
            if (aPart >= Prs3d_DP_XAxis && aPart <= Prs3d_DP_ZAxis)
            {
              Handle(Prs3d_Drawer) aDrawer = aTrihedron->Attributes();
              Handle(Prs3d_DatumAspect) aDatumAspect = aDrawer->DatumAspect();
              Handle(Graphic3d_ArrayOfPrimitives) aPrimitives =
                                                        aDatumAspect->ArrayOfPrimitives(aPart);
              Standard_Real aX1, anY1, aZ1, aX2, anY2, aZ2;
              aPrimitives->Vertice(1, aX1, anY1, aZ1);
              aPrimitives->Vertice(2, aX2, anY2, aZ2);
              gp_Pnt aPnt1(aX1, anY1, aZ1);
              gp_Pnt aPnt2(aX2, anY2, aZ2);
              gp_Lin aLine(aPnt1, gp_Dir(gp_Vec(aPnt1, aPnt2)));
              return aPlane.Contains(aLine, Precision::Confusion(), Precision::Angular());
            }
          }
        }
#endif
        // This is not object controlled by the filter
        aValid = Standard_True;
        }
      }
    }
  }
#ifdef DEBUG_FILTERS
  qDebug(QString("ModuleBase_ShapeDocumentFilter::IsOk = %1").arg(aValid).toStdString().c_str());
#endif
  return aValid;
}
