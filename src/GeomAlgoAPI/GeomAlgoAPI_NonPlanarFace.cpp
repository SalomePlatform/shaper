// Copyright (C) 2014-2023  CEA, EDF
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

#include <GeomAlgoAPI_NonPlanarFace.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <GeomAPI_Shape.h>
#include <Geom_Surface.hxx>

#include <ShapeFix_Face.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//==================================================================================================
GeomAlgoAPI_NonPlanarFace::GeomAlgoAPI_NonPlanarFace(
    const ListOfShape& theEdges)
{
    build(theEdges);
}

//==================================================================================================
void GeomAlgoAPI_NonPlanarFace::build
    (const ListOfShape& theEdges)
{
    myResultFaces.clear();
    setDone(false);
    if (theEdges.empty())
    {
        return;
    }

    //prepare edges
    Handle(TopTools_HSequenceOfShape) aSeqEdgesIn = new TopTools_HSequenceOfShape;
    TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
    TopTools_MapOfShape aMapEdges;
    for (auto anEdge: theEdges) 
    {
        const TopoDS_Edge& aTEdge = anEdge->impl<TopoDS_Edge>();
        if (aMapEdges.Add(aTEdge))
        {
            BRepBuilderAPI_Copy aCopy(aTEdge, Standard_False, Standard_False);
            const TopoDS_Shape& aCopyEdge = aCopy.Shape();
            aSeqEdgesIn->Append(aCopyEdge);
        }
    }

    // no edges
    if (aSeqEdgesIn->IsEmpty()) 
    {
        return;
    }

    // Connect edges to wires of maximum length
    Handle(TopTools_HSequenceOfShape) aSeqWiresOut;
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(aSeqEdgesIn, Precision::Confusion(), Standard_False, aSeqWiresOut);

    // prepare compound result
    BRep_Builder aBuilder;
    TopoDS_Compound aResult;
    aBuilder.MakeCompound(aResult);

    // try to construct face for each non-planar wire
    for(int ind = 1; ind <= aSeqWiresOut->Length(); ind++)
    {
        if(!aSeqWiresOut->Value(ind).Closed())
        {
            continue;
        }
        TopoDS_Wire aWire = TopoDS::Wire(aSeqWiresOut->Value(ind));

        // try to construct filling surface
        BRepOffsetAPI_MakeFilling aMF;
        BRepTools_WireExplorer aWExp(aWire);
        for (; aWExp.More(); aWExp.Next()) 
        {
            aMF.Add(TopoDS::Edge(aWExp.Current()), GeomAbs_C0);
        }
        aMF.Build();
        if (!aMF.IsDone()) 
        {
            return;
        }
        // Result of filling
        TopoDS_Shape aFillRes = aMF.Shape();
        Handle(Geom_Surface) aGS = BRep_Tool::Surface(TopoDS::Face(aFillRes));
        BRepBuilderAPI_MakeFace aMakeFace(aGS, aWire);
        if (aMakeFace.IsDone()) 
        {
            TopoDS_Face aNewFace = aMakeFace.Face();
            Handle(ShapeFix_Face) aFix = new ShapeFix_Face(aNewFace);
            aFix->Perform();
            aFix->FixOrientation();
            aNewFace = aFix->Face();

            //check and fix pcurves, if necessary
            TopExp_Explorer aExpE;
            Standard_Real aT, aTolE, aDMax;
            ShapeFix_ShapeTolerance sat;
            aExpE.Init(aNewFace, TopAbs_EDGE);
            for (; aExpE.More(); aExpE.Next())
            {
                const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExpE.Current();
                if (!BOPTools_AlgoTools::ComputeTolerance(aNewFace, aE, aDMax, aT)) continue;
                aTolE = BRep_Tool::Tolerance(aE);
                if (aDMax < aTolE) continue;

                sat.LimitTolerance(aE, aDMax);
            }

            // store face
            aBuilder.Add(aResult, aNewFace);
            std::shared_ptr<GeomAPI_Shape> aResFace(new GeomAPI_Shape);
            aResFace->setImpl(new TopoDS_Face(aNewFace));
            myResultFaces.push_back(aResFace);
        }
    }
    
    // update results
    GeomShapePtr aResShape(new GeomAPI_Shape);
    aResShape->setImpl(new TopoDS_Shape(aResult));
    setShape(aResShape);
    setDone(true);
}
