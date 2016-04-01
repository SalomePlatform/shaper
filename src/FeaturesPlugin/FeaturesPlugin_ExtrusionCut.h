// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionCut.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_ExtrusionCut_H_
#define FeaturesPlugin_ExtrusionCut_H_

#include "FeaturesPlugin_ExtrusionBoolean.h"

/// \class FeaturesPlugin_ExtrusionCut
/// \ingroup Plugins
/// \brief This feature allows to create sketch, extrude faces from this sketch and
///        cut result from other objects in a single operation.
class FeaturesPlugin_ExtrusionCut : public FeaturesPlugin_ExtrusionBoolean
{
public:
  /// Use plugin manager for features creation.
  FeaturesPlugin_ExtrusionCut();

   /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_EXTRUSION_ID("ExtrusionCut");
    return MY_EXTRUSION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_ExtrusionCut::ID();
    return MY_KIND;
  }
};

#endif
