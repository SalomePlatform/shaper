// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Box.h
// Created:     10 Mar 2016
// Author:      Clarisse Genrault (CEA)

#ifndef PrimitivesPlugin_Box_H_
#define PrimitivesPlugin_Box_H_

#include <PrimitivesPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Box.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/**\class PrimitivesPlugin_Box
 * \ingroup Plugins
 * \brief Feature for creation of a box primitive using various methods.
 *
 * Box creates a cuboid - Parallelepiped with 6 rectangular faces. It can be built via two
 * methods : using two points that define a diagonal, or using 3 lengths that define the 
 * rectangular dimensions.
 */
class PrimitivesPlugin_Box : public ModelAPI_Feature
{
 public:
  /// Box kind
  inline static const std::string& ID()
  {
    static const std::string MY_BOX_ID("Box");
    return MY_BOX_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD_BY_DIMENSIONS()
  {
    static const std::string MY_CREATION_METHOD_ID("BoxByDimensions");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD_BY_TWO_POINTS()
  {
    static const std::string MY_CREATION_METHOD_ID("BoxByTwoPoints");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of first point
  inline static const std::string& POINT_FIRST_ID()
  {
    static const std::string MY_POINT_FIRST_ID("FirstPoint");
    return MY_POINT_FIRST_ID;
  }

  /// Attribute name of second point
  inline static const std::string& POINT_SECOND_ID()
  {
    static const std::string MY_POINT_SECOND_ID("SecondPoint");
    return MY_POINT_SECOND_ID;
  }

  /// Attribute first coordinate
  inline static const std::string& DX_ID()
  {
    static const std::string MY_DX_ID("dx");
    return MY_DX_ID;
  }

  /// Attribute second coordinate
  inline static const std::string& DY_ID()
  {
    static const std::string MY_DY_ID("dy");
    return MY_DY_ID;
  }

  /// Attribute third coordinate
  inline static const std::string& DZ_ID()
  {
    static const std::string MY_DZ_ID("dz");
    return MY_DZ_ID;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Box::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Box();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Box> theBoxAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBox);

  ///Perform the creation of the box using two points defining a diagonal
  void createBoxByTwoPoints();

  ///Perform the creation of the box using three cordinates
  void createBoxByDimensions();

};


#endif
