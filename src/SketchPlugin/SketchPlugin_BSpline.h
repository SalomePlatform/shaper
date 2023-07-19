// Copyright (C) 2019-2023  CEA, EDF
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

#ifndef SketchPlugin_BSpline_H_
#define SketchPlugin_BSpline_H_

#include <SketchPlugin_BSplineBase.h>

/**\class SketchPlugin_BSpline
 * \ingroup Plugins
 * \brief Feature for creation of the B-spline curve in the sketch.
 */
class SketchPlugin_BSpline : public SketchPlugin_BSplineBase
{
public:
  /// B-spline feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchBSpline");
    return ID;
  }

  /// start point of B-spline curve
  inline static const std::string& START_ID()
  {
    static const std::string ID("start_point");
    return ID;
  }
  /// end point of B-spline curve
  inline static const std::string& END_ID()
  {
    static const std::string ID("end_point");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_BSpline::ID();
    return MY_KIND;
  }

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchPlugin_BSpline();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

  virtual bool isPeriodic() const { return false; }
};

#endif
