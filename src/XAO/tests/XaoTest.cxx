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
#include "XaoTest.hxx"
#include "../XAO_XaoUtils.hxx"
#include "../XAO_Xao.hxx"
#include "../XAO_BrepGeometry.hxx"
#include "../XAO_Group.hxx"
#include "../XAO_Field.hxx"

using namespace XAO;

void XaoTest::setUp()
{
}

void XaoTest::tearDown()
{
}

void XaoTest::cleanUp()
{
}

void XaoTest::testGroups()
{
    Xao obj;
    CPPUNIT_ASSERT_THROW(obj.addGroup(XAO::FACE), XAO_Exception);

    BrepGeometry* geom = new BrepGeometry("test");
    obj.setGeometry(geom);
    Group* gr = obj.addGroup(XAO::FACE);
    CPPUNIT_ASSERT_EQUAL(XAO::FACE, gr->getDimension());
    CPPUNIT_ASSERT_EQUAL(1, obj.countGroups());
    Group* gr2 = obj.addGroup(XAO::FACE);
    gr2->setName("AA");

    Group* agr = obj.getGroup(0);
    CPPUNIT_ASSERT(gr == agr);
    CPPUNIT_ASSERT_THROW(obj.getGroup(10), XAO_Exception);

    CPPUNIT_ASSERT_EQUAL(true, obj.removeGroup(gr2));
    CPPUNIT_ASSERT_EQUAL(1, obj.countGroups());

    // remove other group
    Group* gr3 = new Group(XAO::FACE, 3);
    CPPUNIT_ASSERT_EQUAL(false, obj.removeGroup(gr3));
    delete gr3;
}

void XaoTest::testFields()
{
    Xao obj;
    CPPUNIT_ASSERT_THROW(obj.addField(XAO::INTEGER, XAO::FACE, 3), XAO_Exception);

    BrepGeometry* geom = new BrepGeometry("test");
    obj.setGeometry(geom);
    Field* fi = obj.addField(XAO::INTEGER, XAO::FACE, 3);
    CPPUNIT_ASSERT_EQUAL(1, obj.countFields());
    CPPUNIT_ASSERT_EQUAL(XAO::INTEGER, fi->getType());
    CPPUNIT_ASSERT_EQUAL(XAO::FACE, fi->getDimension());
    CPPUNIT_ASSERT_EQUAL(3, fi->countComponents());

    Field* fb = obj.addField(XAO::BOOLEAN, XAO::FACE, 3);
    Field* fd = obj.addField(XAO::DOUBLE, XAO::FACE, 3);
    Field* fs = obj.addField(XAO::STRING, XAO::FACE, 3);
    CPPUNIT_ASSERT_EQUAL(4, obj.countFields());
    CPPUNIT_ASSERT_THROW(obj.getField(10), XAO_Exception);

    CPPUNIT_ASSERT_EQUAL(true, obj.removeField(fb));
    CPPUNIT_ASSERT_EQUAL(3, obj.countFields());


    Field* ff = Field::createField(XAO::INTEGER, XAO::FACE, 3, 3);
    CPPUNIT_ASSERT_EQUAL(false, obj.removeField(ff));
    delete ff;
}

void XaoTest::testFieldsTypes()
{
    Xao obj;
    BrepGeometry* geom = new BrepGeometry("test");
    obj.setGeometry(geom);

    IntegerField* fi = obj.addIntegerField(XAO::FACE, 3);
    BooleanField* fb = obj.addBooleanField(XAO::FACE, 3);
    DoubleField* fd = obj.addDoubleField(XAO::FACE, 3);
    StringField* fs = obj.addStringField(XAO::FACE, 3);

    IntegerField* gfi = obj.getIntegerField(0);
    CPPUNIT_ASSERT(gfi == fi);
    BooleanField* gfb = obj.getBooleanField(1);
    CPPUNIT_ASSERT(gfb == fb);
    DoubleField* gfd = obj.getDoubleField(2);
    CPPUNIT_ASSERT(gfd == fd);
    StringField* gfs = obj.getStringField(3);
    CPPUNIT_ASSERT(gfs == fs);

    CPPUNIT_ASSERT_THROW(obj.getIntegerField(1), XAO_Exception);
    CPPUNIT_ASSERT_THROW(obj.getBooleanField(0), XAO_Exception);
    CPPUNIT_ASSERT_THROW(obj.getDoubleField(0), XAO_Exception);
    CPPUNIT_ASSERT_THROW(obj.getStringField(0), XAO_Exception);
}
