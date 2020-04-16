// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef GDMLPLUGIN_ELLIPSOID_H_
#define GDMLPLUGIN_ELLIPSOID_H_

#include <GDMLPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Ellipsoid.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/**\class GDMLPlugin_Ellipsoid
 * \ingroup Plugins
 * \brief Feature for creation of a GDML Ellipsoid solid.
 */
class GDMLPlugin_Ellipsoid : public ModelAPI_Feature
{
 public:
  /// Cone segment kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONESEGMENT_ID("Ellipsoid");
    return MY_CONESEGMENT_ID;
  }
  /// attribute name of the inner radius at base of cone
  inline static const std::string& AX_ID()
  {
    static const std::string MY_AX_ID("ax");
    return MY_AX_ID;
  }
  /// attribute name of the outer radius at base of cone
  inline static const std::string& BY_ID()
  {
    static const std::string MY_BY_ID("by");
    return MY_BY_ID;
  }
  /// attribute name of the inner radius at top of cone
  inline static const std::string& CZ_ID()
  {
    static const std::string MY_CZ_ID("cz");
    return MY_CZ_ID;
  }
  /// attribute name of the outer radius at top of cone
  inline static const std::string& ZCUT1_ID()
  {
    static const std::string MY_ZCUT1_ID("zcut1");
    return MY_ZCUT1_ID;
  }
  /// attribute name of the outer radius at top of cone
  inline static const std::string& USE_ZCUT1_ID()
  {
    static const std::string MY_USE_ZCUT1_ID("use_zcut1");
    return MY_USE_ZCUT1_ID;
  }
  /// attribute name of the outer radius at top of cone
  inline static const std::string& USE_ZCUT2_ID()
  {
    static const std::string MY_USE_ZCUT2_ID("use_zcut2");
    return MY_USE_ZCUT2_ID;
  }
  /// attribute name of the outer radius at top of cone
  inline static const std::string& ZCUT2_ID()
  {
    static const std::string MY_ZCUT2_ID("zcut2");
    return MY_ZCUT2_ID;
  }

  /// Returns the kind of a feature
  GDMLPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = GDMLPlugin_Ellipsoid::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  GDMLPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  GDMLPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  GDMLPlugin_Ellipsoid();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Ellipsoid> theEllipsoidAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultEllipsoid);

};

#endif // GDMLPLUGIN_ELLIPSOID_H_
