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

#ifndef GeomAlgoAPI_Transform_H_
#define GeomAlgoAPI_Transform_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Trsf.h>

/// \class GeomAlgoAPI_Transform
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by transformating it.
class GeomAlgoAPI_Transform : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Creates an object which is obtained from current object by transformating it.
  /// \param[in] theSourceShape  a shape to be transformed.
  /// \param[in] theTrsf         transformation.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Transform(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Trsf>  theTrsf);

protected:
  /// \brief Default constructor (to be used in the derived classes)
  GeomAlgoAPI_Transform() {}

  /// Builds resulting shape.
  void build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
             std::shared_ptr<GeomAPI_Trsf>  theTrsf);
};

#endif
