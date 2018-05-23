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

#ifndef SketchPlugin_IntersectionPoint_H_
#define SketchPlugin_IntersectionPoint_H_

#include "SketchPlugin_Point.h"

/**\class SketchPlugin_IntersectionPoint
 * \ingroup Plugins
 * \brief Feature for creation of external point as an intersection
 *        between external edge and a plane of the sketch.
 */
class SketchPlugin_IntersectionPoint : public SketchPlugin_Point
{
public:
  /// Point feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_POINT_ID("SketchIntersectionPoint");
    return MY_POINT_ID;
  }
  /// Returns the kind of a feature
  virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_IntersectionPoint::ID();
    return MY_KIND;
  }

  static const std::string& EXTERNAL_FEATURE_ID()
  {
    static std::string MY_FEATURE_ID("ExternalFeature");
    return MY_FEATURE_ID;
  }

  static const std::string& INCLUDE_INTO_RESULT()
  {
    static std::string MY_INCLUDE("IncludeToResult");
    return MY_INCLUDE;
  }

  /// Returns true because intersection point is always external
  virtual bool isFixed()
  { return true; }

  /// Returns true if the feature and the feature results can be displayed.
  /// \return false
  virtual bool canBeDisplayed() const
  { return false; }

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Called on change of any argument-attribute of this object: for external point
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchPlugin_IntersectionPoint();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// \brief Find intersection between a line and a sketch plane
  void computePoint();
};

#endif
