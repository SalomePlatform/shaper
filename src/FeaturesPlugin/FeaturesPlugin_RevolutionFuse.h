// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef FeaturesPlugin_RevolutionFuse_H_
#define FeaturesPlugin_RevolutionFuse_H_

#include "FeaturesPlugin_RevolutionBoolean.h"

/// \class FeaturesPlugin_RevolutionFuse
/// \ingroup Plugins
/// \brief This feature allows to create sketch, revol faces from this sketch and
///        fuse result with other objects in a single operation.
class FeaturesPlugin_RevolutionFuse : public FeaturesPlugin_RevolutionBoolean
{
 public:
  /// Use plugin manager for features creation.
  FeaturesPlugin_RevolutionFuse();

   /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("RevolutionFuse");
    return MY_REVOLUTION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RevolutionFuse::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();
};

#endif
