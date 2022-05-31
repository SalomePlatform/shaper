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

#ifndef INITIALIZATIONPLUGIN_PLUGIN_H_
#define INITIALIZATIONPLUGIN_PLUGIN_H_

#include <InitializationPlugin.h>
#include <ModelAPI_Feature.h>

#include <Events_Loop.h>

class InitializationPlugin_EvalListener;

/**\class InitializationPlugin_Plugin
 * \ingroup Plugins
 * This class is represents a plugin.
 * It's aim is to fulfill the newly created documents with the "origin"
 * construction point (0,0,0) and base planes (x0y, z0y, x0z)
 */
class InitializationPlugin_Plugin : public Events_Listener
{
 public:
  /// Creates plug-in and registers it in the Event Loop
  INITIALIZATIONPLUGIN_EXPORT InitializationPlugin_Plugin();
  /// Destructs the plugin
  INITIALIZATIONPLUGIN_EXPORT virtual ~InitializationPlugin_Plugin() {}
  /// Process the ModelAPI_DocumentCreatedMessage to fulfill a document
  /// from the message with origin and planes
  INITIALIZATIONPLUGIN_EXPORT virtual void processEvent(
    const std::shared_ptr<Events_Message>& theMessage);

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
  FeaturePtr createPoint(DocumentPtr theDoc, const std::wstring& theName,
                         double theX, double theY, double theZ);

  /// Creates an axis which is started from origin point
  /// \param theDoc - document to contain an "axis" feature
  /// \param theOrigin - origin point feature
  /// \param theX - X of direction point
  /// \param theY - Y of direction point
  /// \param theZ - Z of direction point
  FeaturePtr createAxis(DocumentPtr theDoc, FeaturePtr theOrigin,
                        double theX, double theY, double theZ);

 private:
  std::shared_ptr<InitializationPlugin_EvalListener> myEvalListener;
  bool myInitDataModel;
};

#endif

