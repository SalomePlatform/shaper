// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_CommonSharedFaces_H_
#define FeaturesPlugin_CommonSharedFaces_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultGroup.h>


/// \class FeaturesPlugin_CommonSharedFaces
/// \ingroup Plugins
/// \brief Feature to check the shared faces of solid

class FeaturesPlugin_CommonSharedFaces : public ModelAPI_Feature
{
public:
  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute(){};

  /// Return Attribut values of result.
  virtual AttributePtr attributObject() = 0;

  /// Return Attribut values of result.
  virtual AttributePtr attributListFaces() = 0;

  /// Return Attribut values of result.
  virtual AttributePtr attributNumberFaces() = 0;

  /// Return Attribut values of IsCompute.
  virtual AttributePtr attributIsCompute() = 0;

  protected:
  FeaturesPlugin_CommonSharedFaces() {}

  //Set group of faces
  void setFacesGroup(const std::wstring& theName );

  // Update the list of faces
  void updateFaces();

  // the shape studied
  GeomShapePtr myShape;

};

#endif
