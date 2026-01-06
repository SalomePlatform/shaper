// Copyright (C) 2014-2026  CEA, EDF
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

#include "PartSet_MouseProcessor.h"

#include "ModuleBase_IViewWindow.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IViewer.h"
#include "PartSet_Module.h"
#include "PartSet_PreviewSketchPlane.h"
#include "ModelAPI_CompositeFeature.h"
#include <gp_Pnt.hxx>
#include <Aspect_Grid.hxx>
#include <QPoint>
#include <PartSet_Tools.h>


/// @brief
/// During drawing of a line on a sketch, a crash happens if coordinates (theX, theY), returned by \ref PartSet_MouseProcessor::convertPointToLocal,
/// coincide with on-sketch plane coordinates of the closest grid node. What exactly causes this odd behaviour remains unknown.
/// The crash is manifested on all supported Linux OSes, except Debian 11 and, maybe, 12.
/// Even on problematic OSes, the crash does not happen if a debugger (at least LLDB) is attached to Salome with Shaper being compiled in Debug mode.
double slightlyChangeVal(double theVal)
{
  static const double eps = 1e-5;

  if (std::abs(theVal) > 1)
    return (1 + eps) * theVal;
  else
    return theVal + eps;
}


/*static*/ bool PartSet_MouseProcessor::convertPointToLocal(
  ModuleBase_IWorkshop* theWorkshop,
  const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
  ModuleBase_IViewWindow* theWindow,
  const QPoint& theEventPos,
  double& theX, double& theY,
  bool theSnap,
  bool theHighlight,
  bool theAddOffset
) {
  ModuleBase_IViewer* const viewer = theWorkshop->viewer();
  if (!viewer)
    return false;

  const auto aV3dViewer = viewer->v3dViewer();
  const PartSet_Module* module = dynamic_cast<PartSet_Module*>(theWorkshop->module());
  if (!module)
    return false;

  const Handle(V3d_View) view = theWindow->v3dView();

  PartSet_PreviewSketchPlane* previewPlane = module->sketchMgr()->previewSketchPlane();
  if (!theSnap || !aV3dViewer || !aV3dViewer->Grid()->IsActive() || previewPlane->getGridSnappingMode() == PartSet_PreviewSketchPlane::GridSnappingMode::Off) {
    const gp_Pnt mousePoint = PartSet_Tools::convertClickToPoint(theEventPos, view);
    PartSet_Tools::convertTo2D(mousePoint, theSketch, view, theX, theY);
    return true;
  }
  else {
    double closestGridPointX, closestGridPointY, closestGridPointZ;
    view->ConvertToGrid(theEventPos.x(), theEventPos.y(), closestGridPointX, closestGridPointY, closestGridPointZ);
    const gp_Pnt gridPoint = gp_Pnt(closestGridPointX, closestGridPointY, closestGridPointZ);

    if (previewPlane->getGridSnappingMode() == PartSet_PreviewSketchPlane::GridSnappingMode::SnapAnyway) {
      PartSet_Tools::convertTo2D(gridPoint, theSketch, view, theX, theY);

      if (theAddOffset) {
        theX = slightlyChangeVal(theX);
        theY = slightlyChangeVal(theY);
      }

      if (theHighlight) {
        view->Viewer()->ShowGridEcho(view, Graphic3d_Vertex(closestGridPointX, closestGridPointY, closestGridPointZ));
        view->Viewer()->SetGridEcho(true);
        view->RedrawImmediate();
      }
      else {
        view->Viewer()->SetGridEcho(false);
        view->RedrawImmediate();
      }

      return true;
    }
    else /* aPreviewPlane->getGridSnappingMode() == PartSet_PreviewSketchPlane::GridSnappingMode::SnapInProximity */ {
      Standard_Integer aClosestPX, aClosestPY; // Unit is pixel.
      view->Convert(closestGridPointX, closestGridPointY, closestGridPointZ, aClosestPX, aClosestPY);
      const int squareDistanceP = std::pow(aClosestPX - theEventPos.x(), 2) + std::pow(aClosestPY - theEventPos.y(), 2);
      static const int THRESHOLD = std::pow(PartSet_PreviewSketchPlane::SNAP_PROXIMITY_P, 2);

      if (squareDistanceP > THRESHOLD) {
        const gp_Pnt mousePoint = PartSet_Tools::convertClickToPoint(theEventPos, view);
        PartSet_Tools::convertTo2D(mousePoint, theSketch, view, theX, theY);
        view->Viewer()->SetGridEcho(false);
        view->RedrawImmediate();
      }
      else {
        PartSet_Tools::convertTo2D(gridPoint, theSketch, view, theX, theY);

        if (theAddOffset) {
          theX = slightlyChangeVal(theX);
          theY = slightlyChangeVal(theY);
        }

        view->Viewer()->ShowGridEcho(view, Graphic3d_Vertex(closestGridPointX, closestGridPointY, closestGridPointZ));
        view->Viewer()->SetGridEcho(true);
        view->RedrawImmediate();
      }
      return true;
    }
  }
}