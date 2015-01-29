// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Plugin_H_
#define PartSetPlugin_Plugin_H_

#include <PartSetPlugin.h>
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Events.h>

#include <Events_Listener.h>

#include <memory>

/**\class PartSetPlugin_Plugin
 * \ingroup Plugins
 * \brief The main class for management the part set operations as plugin.
 */
class PARTSETPLUGIN_EXPORT PartSetPlugin_Plugin : public ModelAPI_Plugin,
                                                  public Events_Listener
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

  /// Is needed for python wrapping by swig
  PartSetPlugin_Plugin();

  //! Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);
  //! Performs the chenges of enabled/disabled state in the toolbar
  //! due to the current state of the application.
  std::shared_ptr<ModelAPI_FeatureStateMessage> getFeaturesState();
};

#endif
