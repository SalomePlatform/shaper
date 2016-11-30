// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GDMLPlugin_ConeSegment.h
// Created:     23 Nov 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GDMLPLUGIN_CONESEGMENT_H_
#define GDMLPLUGIN_CONESEGMENT_H_

#include <GDMLPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_ConeSegment.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/**\class GDMLPlugin_ConeSegment
 * \ingroup Plugins
 * \brief Feature for creation of a GDML ConeSegment solid.
 */
class GDMLPlugin_ConeSegment : public ModelAPI_Feature
{
 public:
  /// Cone segment kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONESEGMENT_ID("ConeSegment");
    return MY_CONESEGMENT_ID;
  }
  /// attribute name of the inner radius at base of cone
  inline static const std::string& RMIN1_ID()
  {
    static const std::string MY_RMIN1_ID("rmin1");
    return MY_RMIN1_ID;
  }
  /// attribute name of the outer radius at base of cone
  inline static const std::string& RMAX1_ID()
  {
    static const std::string MY_RMAX1_ID("rmax1");
    return MY_RMAX1_ID;
  }
  /// attribute name of the inner radius at top of cone
  inline static const std::string& RMIN2_ID()
  {
    static const std::string MY_RMIN2_ID("rmin2");
    return MY_RMIN2_ID;
  }
  /// attribute name of the outer radius at top of cone
  inline static const std::string& RMAX2_ID()
  {
    static const std::string MY_RMAX2_ID("rmax2");
    return MY_RMAX2_ID;
  }
  /// attribute name of the height of the cone segment
  inline static const std::string& Z_ID()
  {
    static const std::string MY_Z_ID("z");
    return MY_Z_ID;
  }
  /// attribute name of the start angle of the segment
  inline static const std::string& STARTPHI_ID()
  {
    static const std::string MY_STARTPHI_ID("startphi");
    return MY_STARTPHI_ID;
  }
  /// attribute name of the angle of the segment
  inline static const std::string& DELTAPHI_ID()
  {
    static const std::string MY_DELTAPHI_ID("deltaphi");
    return MY_DELTAPHI_ID;
  }

  /// Returns the kind of a feature
  GDMLPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = GDMLPlugin_ConeSegment::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  GDMLPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  GDMLPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  GDMLPlugin_ConeSegment();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_ConeSegment> theConeSegmentAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultConeSegment);

};

#endif // GDMLPLUGIN_CONESEGMENT_H_
