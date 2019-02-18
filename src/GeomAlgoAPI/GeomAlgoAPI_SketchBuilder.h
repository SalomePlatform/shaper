// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <memory>
#include <list>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>

/** \class GeomAlgoAPI_SketchBuilder
 *  \ingroup DataAlgo
 *  \brief Creates planar faces based on the list of Sketch features
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_SketchBuilder
{
 public:
  /** \brief Creates list of faces based on the features of the sketch
   *  \param[in]  theOrigin      origin point of the sketch
   *  \param[in]  theDirX        x-direction of the sketch
   *  \param[in]  theNorm        normal of the sketch
   *  \param[in]  theFeatures    initial features of the sketch
   *  \param[out] theResultFaces faces based on closed wires
   *
   *  The algorithm searches all loops of edges surrounding lesser areas.
   */
  static void createFaces(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                          const std::shared_ptr<GeomAPI_Dir>& theDirX,
                          const std::shared_ptr<GeomAPI_Dir>& theNorm,
                          const std::list<std::shared_ptr<GeomAPI_Shape> >& theFeatures,
                          std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces);

  /** \brief Creates list of faces and unclosed wires on basis of the features of the sketch
   *  \param[in]  theOrigin      origin point of the sketch
   *  \param[in]  theDirX        x-direction of the sketch
   *  \param[in]  theNorm        normal of the sketch
   *  \param[in]  theWire        a wire which contains all edges
   *  \param[out] theResultFaces faces based on closed wires
   *
   *  The algorithm searches all loops of edges surrounding lesser areas.
   */
  static void createFaces(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                          const std::shared_ptr<GeomAPI_Dir>& theDirX,
                          const std::shared_ptr<GeomAPI_Dir>& theNorm,
                          const std::shared_ptr<GeomAPI_Shape>& theWire,
                          std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces);
};

#endif
