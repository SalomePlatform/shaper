// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef GeomAlgoAPI_Boolean_H_
#define GeomAlgoAPI_Boolean_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Boolean
/// \ingroup DataAlgo
/// \brief Allows to perform of boolean operations
class GeomAlgoAPI_Boolean : public GeomAlgoAPI_MakeShape
{
public:

  /// Constructor.
  /// \param[in] theObject the main object.
  /// \param[in] theTool the tool object.
  /// \param[in] theOperationType type of boolean operation.
  /// \param[in] theFuzzy additional tolerance value.
  ///            If the fuzzy value is non-positive, the boolean operation will use a default
  //             additional tolerance value of 1.e-5.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Boolean(const GeomShapePtr theObject,
                                         const GeomShapePtr theTool,
                                         const GeomAlgoAPI_Tools::BOPType theOperationType,
                                         const double theFuzzy = -1);

  /// Constructor.
  /// \param[in] theObject the main object.
  /// \param[in] theTools list of tools.
  /// \param[in] theOperationType type of boolean operation.
  /// \param[in] theFuzzy additional tolerance value.
  ///            If the fuzzy value is non-positive, the boolean operation will use a default
  //             additional tolerance value of 1.e-5.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Boolean(const GeomShapePtr theObject,
                                         const ListOfShape& theTools,
                                         const GeomAlgoAPI_Tools::BOPType theOperationType,
                                         const double theFuzzy = -1);


  /// Constructor.
  /// \param[in] theObjects list of main objects.
  /// \param[in] theTools list of tools.
  /// \param[in] theOperationType type of boolean operation.
  /// \param[in] theFuzzy additional tolerance value.
  ///            If the fuzzy value is non-positive, the boolean operation will use a default
  //             additional tolerance value of 1.e-5.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Boolean(const ListOfShape& theObjects,
                                         const ListOfShape& theTools,
                                         const GeomAlgoAPI_Tools::BOPType theOperationType,
                                         const double theFuzzy = -1);

  /// Redefinition of the generic method for the Fuse problem: OCCT 30481
  GEOMALGOAPI_EXPORT virtual void modified(const GeomShapePtr theOldShape,
                                           ListOfShape& theNewShapes);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theObjects,
             const ListOfShape& theTools,
             const GeomAlgoAPI_Tools::BOPType theOperationType,
             const double theFuzzy);
};

#endif
