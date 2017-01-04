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

#ifndef __XAO_XAO_TEST_HXX__
#define __XAO_XAO_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class XaoTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(XaoTest);
        CPPUNIT_TEST(testGroups);
        CPPUNIT_TEST(testFields);
        CPPUNIT_TEST(testFieldsTypes);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testGroups();
        void testFields();
        void testFieldsTypes();
    };
}

#endif // __XAO_GROUP_TEST_HXX__
