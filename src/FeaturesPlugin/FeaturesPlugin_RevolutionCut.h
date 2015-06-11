// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionCut.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_RevolutionCut_H_
#define FeaturesPlugin_RevolutionCut_H_

#include <FeaturesPlugin_RevolutionBoolean.h>

/** \class FeaturesPlugin_RevolutionCut
 *  \ingroup Plugins
 */
class FeaturesPlugin_RevolutionCut : public FeaturesPlugin_RevolutionBoolean
{
 public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("RevolutionCut");
    return MY_REVOLUTION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RevolutionCut::ID();
    return MY_KIND;
  }

  /// Use plugin manager for features creation.
  FeaturesPlugin_RevolutionCut()
  {
    myBooleanOperationType = GeomAlgoAPI_Boolean::BOOL_CUT;
  }
};

#endif
