// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef __XAO_TESTUTILS_HXX__
#define __XAO_TESTUTILS_HXX__

#include <fstream>
#include <cstdlib>

namespace XAO
{
    class TestUtils
    {
    public:
        static std::string getTestFilePath(const std::string& fileName)
        {
            std::string dataDir = getenv("GEOM_SRC_DIR");
            dataDir += "/src/XAO/tests/data/";
            dataDir += fileName;
            return dataDir;
        }

        static char* readTextFile(const std::string& filePath)
        {
            std::ifstream rstr;
            int length;
            rstr.open(filePath.c_str(), std::ios_base::binary);
            rstr.seekg(0, rstr.end);        // go to the end
            length = rstr.tellg();          // report location (this is the length)
            rstr.seekg(0, rstr.beg);        // go back to the beginning
            char* txt = new char[length+1]; // allocate memory for a buffer of appropriate dimension
            rstr.read(txt, length);         // read the whole file into the buffer
            txt[length] = '\0';
            rstr.close();

            return txt;
        }
    };
}

#endif /* __XAO_TESTUTILS_HXX__ */
