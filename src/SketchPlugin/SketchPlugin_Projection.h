// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SketchPlugin_Projection_H_
#define SketchPlugin_Projection_H_

#include "SketchPlugin_SketchEntity.h"

/** \class SketchPlugin_Projection
 *  \ingroup Plugins
 *  \brief Feature for creation of external feature as a projection onto the sketch plane.
 */
class SketchPlugin_Projection : public SketchPlugin_SketchEntity
{
public:
  /// Projection feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_PROJECTION_ID("SketchProjection");
    return MY_PROJECTION_ID;
  }
  /// Returns the kind of a feature
  virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Projection::ID();
    return MY_KIND;
  }

  static const std::string& EXTERNAL_FEATURE_ID()
  {
    static std::string MY_EXT_FEATURE_ID("ExternalFeature");
    return MY_EXT_FEATURE_ID;
  }

  static const std::string& PROJECTED_FEATURE_ID()
  {
    static std::string MY_PROJ_FEATURE_ID("ProjectedFeature");
    return MY_PROJ_FEATURE_ID;
  }

  /// Returns true because projected feature is always external
  virtual bool isFixed()
  { return true; }

  /// Returns true if the feature and the feature results can be displayed.
  /// \return false
  SKETCHPLUGIN_EXPORT virtual bool canBeDisplayed() const
  {
    return false;
  }

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Called on change of any argument-attribute of this object: for external point
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchPlugin_Projection();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// \brief Find projection of a feature onto sketch plane
  void computeProjection(const std::string& theID);

  bool myIsComputing;
};

#endif
