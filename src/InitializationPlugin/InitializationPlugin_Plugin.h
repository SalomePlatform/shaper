
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef INITIALIZATIONPLUGIN_PLUGIN_H_
#define INITIALIZATIONPLUGIN_PLUGIN_H_

#include <InitializationPlugin.h>

#include <ModelAPI_Feature.h>

#include <Events_Loop.h>

/**\class InitializationPlugin_Plugin
 * TODO: Add documentation
 */
class INITIALIZATIONPLUGIN_EXPORT InitializationPlugin_Plugin : public Events_Listener
{
 public:
  InitializationPlugin_Plugin();
  ~InitializationPlugin_Plugin() {}
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  void createPlane(DocumentPtr theDoc, double theA, double theB, double theC);
  void createPoint(DocumentPtr theDoc);
};

#endif

