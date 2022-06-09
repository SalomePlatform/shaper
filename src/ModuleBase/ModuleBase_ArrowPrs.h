// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef ModuleBase_ArrowPrs_H
#define ModuleBase_ArrowPrs_H

#include <ModuleBase.h>
#include <GeomAPI_Edge.h>
#include <AIS_InteractiveContext.hxx>

#include <map>

DEFINE_STANDARD_HANDLE(ModuleBase_ArrowPrs, AIS_InteractiveContext)

/**
* \ingroup GUI
* A presentation class for displaying a direction of edge
*/

class ModuleBase_ArrowPrs : public AIS_InteractiveContext
{
public:
  /// Constructor
  /// \param theViewer a viewer which theEdge is displaying.
  /// \param theEdge an edge whose direction to display.
  Standard_EXPORT ModuleBase_ArrowPrs(const Handle(V3d_Viewer)& theViewer,
                                      const GeomEdgePtr& theEdge);

  /// Returns an edge shape
  GeomEdgePtr Edge() const { return myEdge; }

  /// Draw arrow that represent direction of the edge.
  Standard_EXPORT void DrawArrow(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor);

  /// \brief Compare addresses of edges
  class Comparator
  {
  public:
    /// Return \c true if the address of the first edge is less than the address of the second
    MODULEBASE_EXPORT
      bool operator ()(const std::shared_ptr<GeomAPI_Edge>& theEdge1,
                       const std::shared_ptr<GeomAPI_Edge>& theEdge2) const;
  };

  DEFINE_STANDARD_RTTIEXT(ModuleBase_ArrowPrs, AIS_InteractiveContext)

private:

  /// The edge whose direction to display.
  GeomEdgePtr myEdge;
};

typedef std::pair<GeomEdgePtr, Handle(ModuleBase_ArrowPrs)> EdgeDirection;
typedef std::map<GeomEdgePtr, Handle(ModuleBase_ArrowPrs), ModuleBase_ArrowPrs::Comparator> EdgesDirectionMap;

#endif