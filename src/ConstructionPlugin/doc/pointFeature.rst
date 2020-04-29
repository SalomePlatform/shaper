.. _constructionPoint:
.. |point_button.icon|    image:: images/point_button.png

Point
=====

Point feature creates a new construction point.

Point is a construction object and it can be created in a part or in a partset. To create a point:

#. select in the Main Menu *Construction - > Point* item  or
#. click |point_button.icon| **Point** button in the toolbar

There are 5 algorithms for creation of a Point:

.. image:: images/point_by_xyz_32x32.png
   :align: left
   :height: 24px

**By X,Y,Z** creates a point by three coordinates.

.. image:: images/point_by_distance_on_edge_32x32.png
   :align: left
   :height: 24px

**By distance on edge** creates a point on an edge.

.. image:: images/point_by_projection_32x32.png
   :align: left
   :height: 24px

**By projection on edge or plane** creates a point by projecting an existing point on an edge or plane.

.. image:: images/point_by_intersection_32x32.png
   :align: left
   :height: 24px

**By intersection of objects** creates a point by intersection of planes or/ and edges.

.. image:: images/point_by_geometrical_property_32x32.png
   :align: left
   :height: 24px

**By geometrical property of object** creates a point in COG or center of arc or circle.

--------------------------------------------------------------------------------

By X,Y,Z
--------

.. image:: images/Point1.png
   :align: center
	
.. centered::
   **By X,Y,Z coordinates**

A point is created by X, Y, and Z coordinates.

**TUI Commands**:

.. py:function:: model.addPoint(Part_doc, 50, 50, 50)

    :param part: The current part object.
    :param real: X value.
    :param real: Y value.
    :param real: Z value.
    :return: Result object.

Result
""""""

The Result of the operation will be a construction point:

.. image:: images/CreatePoint1.png
	   :align: center

.. centered::
   **Point by coordinates**

**See Also** a sample TUI Script of :ref:`tui_create_point_xyz` operation.

By distance on edge
-------------------

.. image:: images/Point2.png
   :align: center
	
.. centered::
   **Along an edge**

To create a point, select an edge in a viewer and define a distance along the edge, where point will be defined. This distance can be defined by an absolute value or by a relative one as a ratio to the edge length. The direction of the edge can be reversed by the corresponding check-box.

**TUI Commands**:

.. py:function:: model.addPoint(Part_doc, model.selection("EDGE", "Box_1_1/Left&Box_1_1/Top"), 0.5, True, False)

    :param part: The current part object.
    :param object: An edge.
    :param real: A value.
    :param boolean: Is by ratio.
    :param boolean: Is reversed.
    :return: Result object.

Result
""""""

The Result of the operation will be a construction point created on edge:

.. image:: images/CreatePoint2.png
	   :align: center

.. centered::
   **Point created on edge**

**See Also** a sample TUI Script of :ref:`tui_create_point_edge` operation.

By projection on edge or plane
------------------------------

To create a point by projection it is necessary to select an existing point or wertex which will be projected and an edge or a plane (planar face) on which it will be projected:

**On an edge:**

.. image:: images/Point3.png
   :align: center
	
.. centered::
   **By projection on an edge**

**On a plane:**

.. image:: images/Point3-1.png
   :align: center
	
.. centered::
   **By projection on a plane**


The new point will be created by projection of the selected point on the selected object.

**TUI Commands**:

.. py:function:: model.addPoint(Part_doc, model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Left&Box_1_1/Top"), model.selection("EDGE", "Box_1_1/Right&Box_1_1/Top"))

    :param part: The current part object.
    :param object: A vertex.
    :param object: An edge or plane.
    :return: Result object.

Result
""""""

The Result of the operation will be a construction point created by projection on a plane or an edge:

.. image:: images/CreatePoint3.png
	   :align: center

.. centered::
   **Point created by projection on a plane**

**See Also** a sample TUI Script of :ref:`tui_create_point_projection` operation.

By intersection of objects
--------------------------

A point can be created by intersection of selected objects:

**Two edges**

.. image:: images/Point4.png
   :align: center
	
.. centered::
   **Intersection of edges**

**Edge and plane**

.. image:: images/Point4-1.png
   :align: center
	
.. centered::
   **Intersection of and edge and a plane**

In this case it is possible to define an offset from a plane along the plane normal for the created point.

**Three planes**

.. image:: images/Point4-2.png
   :align: center
	
.. centered::
   **Intersection of three planes**

The new point will be defined by intersection of the selected objects.

**TUI Commands**:

.. py:function:: model.addPoint(Part_doc, model.selection("EDGE", "Box_1_1/Front&Box_1_1/Top"), model.selection("FACE", "Box_1_1/Left"), 10, False)

    :param part: The current part object.
    :param object: An edge.
    :param object: A plane.
    :param real: Offset.
    :param boolean: Is offset reversed.
    :return: Result object.

Result
""""""

The Result of the operation will be a construction point created by intersection of objects:

.. image:: images/CreatePoint4.png
	   :align: center

.. centered::
   **Point created by intersection of axis**

**See Also** a sample TUI Script of :ref:`tui_create_point_intersection` operation.

By geometrical property of object
---------------------------------

It is possible to use the following property of selected object:

**A center of gravity**

.. image:: images/Point5.png
   :align: center
	
.. centered::
   **By center of gravity**

**A center of circle or arc**

.. image:: images/Point5-1.png
   :align: center
	
.. centered::
   **By center of a circle**
   
To create a point, select a desirable object.

**TUI Commands**:

.. py:function:: model.addPoint(Part_1_doc, model.selection("SOLID", "Box_1_1"))

    :param part: The current part object.
    :param object: Solid.
    :return: Result object.

Result
""""""

The Result of the operation will be a construction point defined by object property:

.. image:: images/CreatePoint5.png
	   :align: center

.. centered::
   **Point created in COG of a cylinder**

**See Also** a sample TUI Script of :ref:`tui_create_point_object` operation.
