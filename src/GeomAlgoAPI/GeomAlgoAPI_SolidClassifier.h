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

#ifndef GeomAlgoAPI_SolidClassifier_H_
#define GeomAlgoAPI_SolidClassifier_H_

#include <GeomAlgoAPI.h>

#include <memory>

class GeomAPI_Shape;
class GeomAPI_Solid;

/// \class GeomAlgoAPI_SolidClassifier
/// \ingroup DataAlgo
/// \brief Classify shape according to the given solid.
class GeomAlgoAPI_SolidClassifier
{
public:
  typedef int State;

  static const State State_UNKNOWN = 0x0;
  static const State State_IN      = 0x1;
  static const State State_ON      = 0x2;
  static const State State_OUT     = 0x4;
  static const State State_ALL     = State_IN & State_ON & State_OUT;

public:
  /// \brief Perform classification of the shape according to the solid.
  /// \param[in] theSolid       the base solid
  /// \param[in] theShape       the shape to classify
  /// \param[in] theTolerance   comparison tolrence
  GEOMALGOAPI_EXPORT GeomAlgoAPI_SolidClassifier(const std::shared_ptr<GeomAPI_Solid> theSolid,
                                                 const std::shared_ptr<GeomAPI_Shape> theShape,
                                                 const double theTolerance = 1.e-7);

  /// \return Classification result.
  State state() const { return myState; }

private:
  State myState;
};

#endif
