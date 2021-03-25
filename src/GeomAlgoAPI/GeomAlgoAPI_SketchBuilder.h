// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_SketchBuilder_H_
#define GeomAlgoAPI_SketchBuilder_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <memory>
#include <list>

class GeomAPI_Dir;
class GeomAPI_Pln;
class GeomAPI_Pnt;
class GeomAPI_Shape;

/** \class GeomAlgoAPI_SketchBuilder
 *  \ingroup DataAlgo
 *  \brief Creates planar faces based on the list of Sketch features
 */
class GeomAlgoAPI_SketchBuilder : public GeomAlgoAPI_MakeShape
{
public:
  /** \brief Creates list of faces based on the edges of the sketch
   *  \param[in]  thePlane  plane of the sketch
   *  \param[in]  theEdges  initial edges of the sketch
   *
   *  The algorithm searches all loops of edges surrounding lesser areas.
   */
  GEOMALGOAPI_EXPORT
  GeomAlgoAPI_SketchBuilder(const std::shared_ptr<GeomAPI_Pln>& thePlane,
                            const std::list<std::shared_ptr<GeomAPI_Shape> >& theEdges);

  /** \brief Creates list of faces and unclosed wires on basis of the features of the sketch
   *  \param[in]  theOrigin      origin point of the sketch
   *  \param[in]  theDirX        x-direction of the sketch
   *  \param[in]  theNorm        normal of the sketch
   *  \param[in]  theWire        a wire which contains all edges
   *
   *  The algorithm searches all loops of edges surrounding lesser areas.
   */
  GEOMALGOAPI_EXPORT
  GeomAlgoAPI_SketchBuilder(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                            const std::shared_ptr<GeomAPI_Dir>& theDirX,
                            const std::shared_ptr<GeomAPI_Dir>& theNorm,
                            const std::shared_ptr<GeomAPI_Shape>& theWire);

  /// Return list of created faces
  GEOMALGOAPI_EXPORT const std::list<std::shared_ptr<GeomAPI_Shape> >& faces() const
  { return myResultFaces; }

private:
  void build(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
             const std::shared_ptr<GeomAPI_Dir>& theDirX,
             const std::shared_ptr<GeomAPI_Dir>& theNorm,
             const std::list<std::shared_ptr<GeomAPI_Shape> >& theEdges);

private:
  std::list<std::shared_ptr<GeomAPI_Shape> > myResultFaces;
};

#endif
