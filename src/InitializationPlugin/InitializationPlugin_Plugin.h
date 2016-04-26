// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef INITIALIZATIONPLUGIN_PLUGIN_H_
#define INITIALIZATIONPLUGIN_PLUGIN_H_

#include <InitializationPlugin.h>

#include <ModelAPI_Feature.h>

#include <Events_Loop.h>

/**\class InitializationPlugin_Plugin
 * \ingroup Plugins
 * This class is represents a plugin.
 * It's aim is to fulfill the newly created documents with the "origin"
 * construction point (0,0,0) and base planes (x0y, z0y, x0z)
 */
class INITIALIZATIONPLUGIN_EXPORT InitializationPlugin_Plugin : public Events_Listener
{
 public:
  /// Creates plug-in and registers it in the Event Loop
  InitializationPlugin_Plugin();
  /// Destructs the plugin
  virtual ~InitializationPlugin_Plugin() {}
  /// Process the ModelAPI_DocumentCreatedMessage to fulfill a document
  /// from the message with origin and planes
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

 protected:
  /// Creates a plane by given parameters A, B, C
  /// \param theDoc - document to contain a "plane" feature
  /// \param theX - determines if X is 0 or not
  /// \param theY - determines if Y is 0 or not
  /// \param theZ - determines if Z is 0 or not
  FeaturePtr createPlane(DocumentPtr theDoc, double theX, double theY, double theZ);
  /// Creates the origin point in (0,0,0)
  /// \param theDoc - document to contain a "point" feature
  /// \param theName - name of the point
  /// \param theX - X coordinate
  /// \param theY - Y coordinate
  /// \param theZ - Z coordinate
  FeaturePtr createPoint(DocumentPtr theDoc, const std::string& theName, 
                         double theX, double theY, double theZ);

  /// Creates an axis which is started from origin point
  /// \param theDoc - document to contain an "axis" feature
  /// \param theOrigin - origin point feature
  /// \param theX - X of direction point
  /// \param theY - Y of direction point
  /// \param theZ - Z of direction point
  FeaturePtr createAxis(DocumentPtr theDoc, FeaturePtr theOrigin, 
                        double theX, double theY, double theZ);
};

#endif

