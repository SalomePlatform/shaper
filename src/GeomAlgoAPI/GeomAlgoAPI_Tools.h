// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef GEOMALGOAPI_TOOLS_H_
#define GEOMALGOAPI_TOOLS_H_

#include <GeomAlgoAPI.h>

#include <memory>
#include <string>

class GeomAlgoAPI_MakeShape;

namespace GeomAlgoAPI_Tools {
/// Type of booelan operation
enum BOPType {
  BOOL_CUT,      ///< Cut objects
  BOOL_FUSE,     ///< Fuse objects
  BOOL_COMMON,   ///< Take common part of objects
  BOOL_PARTITION ///< Parition operation
};

/** \class Localizer
 *  \ingroup DataAlgo
 *  \brief Localizer tool.
 */
class Localizer
{
public:
  GEOMALGOAPI_EXPORT Localizer();
  GEOMALGOAPI_EXPORT ~Localizer();
private:
  std::string myCurLocale;
};

/** \class File_Tools
 *  \ingroup DataAlgo
 *  \brief File tool.
 */
class File_Tools {
public:
  /**
   * Returns an extension of theFileName
   */
  GEOMALGOAPI_EXPORT static std::string extension(const std::string& theFileName);
  /**
   * Returns a name of theFileName
   */
  GEOMALGOAPI_EXPORT static std::string name(const std::string& theFileName);
};

/** \class AlgoError
 *  \ingroup DataAlgo
 *  \brief Verify error in MakeShape algorithm.
 */
class AlgoError {
public:
  /** \brief Verify MakeShape algorithm for failures
   *  \param[in]  theAlgorithm object to verify the failure
   *  \param[in]  theFeature   kind of the feature, the algorithm belongs to
   *  \param[out] theError     error string (empty if the feature succeed)
   *  \return \c true if succeed
   */
  GEOMALGOAPI_EXPORT static bool isAlgorithmFailed(
      const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgorithm,
      const std::string& theFeature,
      std::string& theError);
};

} // GeomAlgoAPI_Tools

#endif /* GEOMALGOAPI_TOOLS_H_ */
