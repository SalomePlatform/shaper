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

#include "XaoUtilsTest.hxx"
#include "../XAO_Xao.hxx"
#include "../XAO_XaoUtils.hxx"

using namespace XAO;


void XaoUtilsTest::setUp()
{
}

void XaoUtilsTest::tearDown()
{
}

void XaoUtilsTest::cleanUp()
{
}

void XaoUtilsTest::testBoolean()
{
    CPPUNIT_ASSERT_EQUAL(std::string("true"), XaoUtils::booleanToString(true));
    CPPUNIT_ASSERT_EQUAL(std::string("false"), XaoUtils::booleanToString(false));

    CPPUNIT_ASSERT_EQUAL(true, XaoUtils::stringToBoolean("true"));
    CPPUNIT_ASSERT_EQUAL(true, XaoUtils::stringToBoolean("1"));
    CPPUNIT_ASSERT_EQUAL(false, XaoUtils::stringToBoolean("false"));
    CPPUNIT_ASSERT_EQUAL(false, XaoUtils::stringToBoolean("0"));
    CPPUNIT_ASSERT_THROW(XaoUtils::stringToBoolean("abc"), XAO_Exception);
}

void XaoUtilsTest::testInteger()
{
    CPPUNIT_ASSERT_EQUAL(std::string("0"), XaoUtils::intToString(0));
    CPPUNIT_ASSERT_EQUAL(std::string("123"), XaoUtils::intToString(123));

    CPPUNIT_ASSERT_EQUAL(123, XaoUtils::stringToInt("123"));
    CPPUNIT_ASSERT_THROW(XaoUtils::stringToInt("abc"), XAO_Exception);
}

void XaoUtilsTest::testDouble()
{
    CPPUNIT_ASSERT_EQUAL(std::string("0"), XaoUtils::doubleToString(0));
    CPPUNIT_ASSERT_EQUAL(std::string("12.3"), XaoUtils::doubleToString(12.3));

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.123, XaoUtils::stringToDouble("0.123"), 1e-3);
    CPPUNIT_ASSERT_THROW(XaoUtils::stringToDouble("abc"), XAO_Exception);
}

void XaoUtilsTest::testDimension()
{
    CPPUNIT_ASSERT_EQUAL(std::string("vertex"), XaoUtils::dimensionToString(XAO::VERTEX));
    CPPUNIT_ASSERT_EQUAL(std::string("edge"), XaoUtils::dimensionToString(XAO::EDGE));
    CPPUNIT_ASSERT_EQUAL(std::string("face"), XaoUtils::dimensionToString(XAO::FACE));
    CPPUNIT_ASSERT_EQUAL(std::string("solid"), XaoUtils::dimensionToString(XAO::SOLID));
    CPPUNIT_ASSERT_EQUAL(std::string("whole"), XaoUtils::dimensionToString(XAO::WHOLE));

    CPPUNIT_ASSERT_EQUAL(XAO::VERTEX, XaoUtils::stringToDimension("vertex"));
    CPPUNIT_ASSERT_EQUAL(XAO::EDGE, XaoUtils::stringToDimension("edge"));
    CPPUNIT_ASSERT_EQUAL(XAO::FACE, XaoUtils::stringToDimension("face"));
    CPPUNIT_ASSERT_EQUAL(XAO::SOLID, XaoUtils::stringToDimension("solid"));
    CPPUNIT_ASSERT_EQUAL(XAO::WHOLE, XaoUtils::stringToDimension("whole"));
    CPPUNIT_ASSERT_THROW(XaoUtils::stringToDimension("zz"), XAO_Exception);
}

void XaoUtilsTest::testType()
{
    CPPUNIT_ASSERT_EQUAL(std::string("boolean"), XaoUtils::fieldTypeToString(XAO::BOOLEAN));
    CPPUNIT_ASSERT_EQUAL(std::string("integer"), XaoUtils::fieldTypeToString(XAO::INTEGER));
    CPPUNIT_ASSERT_EQUAL(std::string("double"), XaoUtils::fieldTypeToString(XAO::DOUBLE));
    CPPUNIT_ASSERT_EQUAL(std::string("string"), XaoUtils::fieldTypeToString(XAO::STRING));

    CPPUNIT_ASSERT_EQUAL(XAO::BOOLEAN, XaoUtils::stringToFieldType("boolean"));
    CPPUNIT_ASSERT_EQUAL(XAO::INTEGER, XaoUtils::stringToFieldType("integer"));
    CPPUNIT_ASSERT_EQUAL(XAO::DOUBLE, XaoUtils::stringToFieldType("double"));
    CPPUNIT_ASSERT_EQUAL(XAO::STRING, XaoUtils::stringToFieldType("string"));
    CPPUNIT_ASSERT_THROW(XaoUtils::stringToFieldType("zz"), XAO_Exception);
}

void XaoUtilsTest::testFormat()
{
    CPPUNIT_ASSERT_EQUAL(std::string("BREP"), XaoUtils::shapeFormatToString(XAO::BREP));
    CPPUNIT_ASSERT_EQUAL(std::string("STEP"), XaoUtils::shapeFormatToString(XAO::STEP));

    CPPUNIT_ASSERT_EQUAL(XAO::BREP, XaoUtils::stringToShapeFormat("BREP"));
    CPPUNIT_ASSERT_EQUAL(XAO::STEP, XaoUtils::stringToShapeFormat("STEP"));
    CPPUNIT_ASSERT_THROW(XaoUtils::stringToShapeFormat("zz"), XAO_Exception);
}
