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

#ifndef __XAO_UTILS_TEST_HXX__
#define __XAO_UTILS_TEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace XAO
{
    class XaoUtilsTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(XaoUtilsTest);
        CPPUNIT_TEST(testBoolean);
        CPPUNIT_TEST(testInteger);
        CPPUNIT_TEST(testDouble);
        CPPUNIT_TEST(testDimension);
        CPPUNIT_TEST(testType);
        CPPUNIT_TEST(testFormat);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();
        void cleanUp();

        void testBoolean();
        void testInteger();
        void testDouble();
        void testDimension();
        void testType();
        void testFormat();
    };
}

#endif // __XAO_FIELD_TEST_HXX__
