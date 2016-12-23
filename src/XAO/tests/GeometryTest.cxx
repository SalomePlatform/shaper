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

#include <vector>

#include "TestUtils.hxx"
#include "GeometryTest.hxx"
#include "../XAO_XaoUtils.hxx"
#include "../XAO_Geometry.hxx"
#include "../XAO_GeometricElement.hxx"

using namespace XAO;

void GeometryTest::setUp()
{
}

void GeometryTest::tearDown()
{
}

void GeometryTest::cleanUp()
{
}

void GeometryTest::testGeometryElement()
{
    GeometricElement elt;
    CPPUNIT_ASSERT_EQUAL(false, elt.hasName());
    CPPUNIT_ASSERT_EQUAL(std::string(""), elt.getName());
    CPPUNIT_ASSERT_EQUAL(std::string(""), elt.getReference());

    elt.setName("test");
    CPPUNIT_ASSERT_EQUAL(true, elt.hasName());
    CPPUNIT_ASSERT_EQUAL(std::string("test"), elt.getName());

    elt.setReference("abc");
    CPPUNIT_ASSERT_EQUAL(std::string("abc"), elt.getReference());

    GeometricElement elt2("aa", "bb");
    CPPUNIT_ASSERT_EQUAL(std::string("aa"), elt2.getName());
    CPPUNIT_ASSERT_EQUAL(std::string("bb"), elt2.getReference());
}

void GeometryTest::testGeometryElementList()
{
    GeometricElementList lst;
    CPPUNIT_ASSERT_EQUAL(0, lst.getSize());
    lst.setSize(10);
    CPPUNIT_ASSERT_EQUAL(10, lst.getSize());

    GeometricElementList otherLst(15);
    CPPUNIT_ASSERT_EQUAL(15, otherLst.getSize());
    CPPUNIT_ASSERT_EQUAL(std::string(""), otherLst.getName(0));
    CPPUNIT_ASSERT_EQUAL(std::string(""), otherLst.getReference(0));

    CPPUNIT_ASSERT_THROW(otherLst.getName(20), XAO_Exception);
    CPPUNIT_ASSERT_THROW(otherLst.getReference(20), XAO_Exception);
    CPPUNIT_ASSERT_THROW(otherLst.hasName(20), XAO_Exception);

    otherLst.setName(0, "aa");
    CPPUNIT_ASSERT_EQUAL(std::string("aa"), otherLst.getName(0));
    CPPUNIT_ASSERT_THROW(otherLst.setName(20, "aa"), XAO_Exception);
    otherLst.setReference(0, "bb");
    CPPUNIT_ASSERT_EQUAL(std::string("bb"), otherLst.getReference(0));
    CPPUNIT_ASSERT_THROW(otherLst.setReference(20, "aa"), XAO_Exception);

    otherLst.setSize(10);
    CPPUNIT_ASSERT_EQUAL(std::string(""), otherLst.getName(0));
    CPPUNIT_ASSERT_EQUAL(std::string(""), otherLst.getReference(0));
    CPPUNIT_ASSERT_EQUAL(false, otherLst.hasName(0));

    otherLst.setElement(3, "name", "ref");
    CPPUNIT_ASSERT_EQUAL(std::string("name"), otherLst.getName(3));
    CPPUNIT_ASSERT_EQUAL(std::string("ref"), otherLst.getReference(3));
    CPPUNIT_ASSERT_EQUAL(true, otherLst.hasName(3));
    CPPUNIT_ASSERT_THROW(otherLst.setElement(30, "name", "ref"), XAO_Exception);

    // ---- create elements "name i", "Ri"
    for (int i = 0; i < otherLst.getSize(); ++i)
    {
        std::ostringstream name;
        name << "name " << i;
        std::ostringstream ref;
        ref << "R" << i;

        otherLst.setElement(i, name.str(), ref.str());
    }

    CPPUNIT_ASSERT_EQUAL(8, otherLst.getIndexByReference("R8"));
    CPPUNIT_ASSERT_THROW(otherLst.getIndexByReference("ZZ"), XAO_Exception);

    GeometricElementList::iterator first = otherLst.begin();
    GeometricElement firstElt = first->second;
    CPPUNIT_ASSERT_EQUAL(std::string("R0"),  firstElt.getReference());
}

void GeometryTest::testGeometry()
{
    Geometry* geom = Geometry::createGeometry(XAO::BREP, "cube");

    CPPUNIT_ASSERT_EQUAL(std::string("cube"), geom->getName());
    CPPUNIT_ASSERT_EQUAL(XAO::BREP, geom->getFormat());

    geom->setName("sphere");
    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), geom->getName());

    delete geom;
}

void GeometryTest::testGeometryErrors()
{
    CPPUNIT_ASSERT_THROW(Geometry::createGeometry(XAO::STEP), XAO_Exception);
}

void GeometryTest::testSetElement()
{
    Geometry* geom = Geometry::createGeometry(XAO::BREP, "cube");

    CPPUNIT_ASSERT_THROW(geom->setVertexName(0, "aa"), XAO_Exception);

    char* txt = TestUtils::readTextFile(TestUtils::getTestFilePath("Box_1.brep"));
    geom->setShapeString(txt);

    CPPUNIT_ASSERT_EQUAL(false, geom->hasVertexName(0));
    geom->setVertexName(0, "va");
    CPPUNIT_ASSERT_EQUAL(true, geom->hasVertexName(0));
    CPPUNIT_ASSERT_EQUAL(std::string("va"), geom->getVertexName(0));
    CPPUNIT_ASSERT_THROW(geom->getVertexName(100), XAO_Exception);
    CPPUNIT_ASSERT_THROW(geom->hasVertexName(100), XAO_Exception);

    geom->setEdgeName(0, "ea");
    CPPUNIT_ASSERT_EQUAL(std::string("ea"), geom->getEdgeName(0));
    CPPUNIT_ASSERT_THROW(geom->getEdgeName(100), XAO_Exception);

    geom->setFaceName(0, "fa");
    CPPUNIT_ASSERT_EQUAL(std::string("fa"), geom->getFaceName(0));
    CPPUNIT_ASSERT_THROW(geom->getFaceName(100), XAO_Exception);

    geom->setSolidName(0, "sa");
    CPPUNIT_ASSERT_EQUAL(std::string("sa"), geom->getSolidName(0));
    CPPUNIT_ASSERT_THROW(geom->getSolidName(100), XAO_Exception);

    delete geom;
}
