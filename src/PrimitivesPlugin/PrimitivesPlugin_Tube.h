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

#ifndef PrimitivesPlugin_Tube_H_
#define PrimitivesPlugin_Tube_H_

#include <GeomAlgoAPI_Tube.h>
#include <ModelAPI_Feature.h>
#include <PrimitivesPlugin.h>

/**\class PrimitivesPlugin_Tube
 * \ingroup Plugins
 * \brief Feature for creation of a tube primitive using various methods.
 *
 * Tube creates a tuboid - Holed cylinder. It can be built via two methods : 
 * using an inner raidus, an outer radius and a heigth or radii (inner and outer)
 * and angles.
 */
class PrimitivesPlugin_Tube : public ModelAPI_Feature
{
 public:
  /// Tube kind
  inline static const std::string& ID()
  {
    static const std::string MY_TUBE_ID("Tube");
    return MY_TUBE_ID;
  }

  /// Attrinute name of the inner radius
  inline static const std::string& RMIN_ID()
  {
    static const std::string MY_RMIN_ID("rmin");
    return MY_RMIN_ID;
  }

  /// Attrinute name of the outer radius
  inline static const std::string& RMAX_ID()
  {
    static const std::string MY_RMAX_ID("rmax");
    return MY_RMAX_ID;
  }

  /// Attrinute name of the height
  inline static const std::string& HEIGHT_ID()
  {
    static const std::string MY_HEIGHT_ID("height");
    return MY_HEIGHT_ID;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Tube::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  PrimitivesPlugin_Tube();

 private:
  /// Load naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Tube> theTubeAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBox);
};

#endif
