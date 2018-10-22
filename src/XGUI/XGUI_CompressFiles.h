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

#ifndef XGUI_CompressFiles_H
#define XGUI_CompressFiles_H

#include "XGUI.h"

#include <QString>

#include <list>

/**
* \ingroup GUI
* The class implements static methods for compressing/uncompressing files
*/
class XGUI_CompressFiles
{
public:

  /// Compress list of file to an archive
  /// \param theFile a name of compressed output file
  /// \param theFiles a list of input files to compress
  static bool compress(const QString& theFile, std::list<std::string> theFiles);

  /// Uncompress files
  /// \param theFile a compressed file
  /// \param theDir an output directory for uncompressed files
  static bool uncompress(const QString& theFile, const QString& theDir);

};

#endif