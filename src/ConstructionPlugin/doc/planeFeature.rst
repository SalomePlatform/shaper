.. |plane_button.icon|    image:: images/plane_button.png

Plane
=====

Plane feature creates a new constructive plane.

Plane is a construction object that can be created in a part or in a partset. To create a plane:

#. select in the Main Menu *Construction - > Plane* item  or
#. click |plane_button.icon| **Point** button in the toolbar

There are 4 algorithms for creation of a Plane:

.. figure:: images/plane_by_three_points_32x32.png
   :align: left
   :height: 24px

**By three points** creates a plane by three points.

.. figure:: images/plane_by_line_and_point_32x32.png
   :align: left
   :height: 24px

**By line and point** creates a plane by a line and a point.

.. figure:: images/plane_by_other_plane_32x32.png
   :align: left
   :height: 24px

**By other plane** creates a plane parallel to another plane.

.. figure:: images/plane_by_two_parallel_planes_32x32.png
   :align: left
   :height: 24px

**By two parallel planes** creates a plane between two parallel planes.

--------------------------------------------------------------------------------

By three points
---------------

.. figure:: images/Plane1.png
   :align: center

   **By three points**

A plane is created by selecting three points in a viewer.

**TUI Commands**:

.. py:function:: model.addPlane(Part_doc, model.selection("VERTEX", "Box_1_1/Back&Box_1_1/Left&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Right&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Left&Box_1_1/Bottom"))

    :param part: The current part object.
    :param object: Vertex 1.
    :param object: Vertex 2.
    :param object: Vertex 3.
    :return: Result object.

Result
""""""

The Result of the operation will be a plane:

.. figure:: images/CreatedPlane1.png
   :align: center

   **A plane by three points**

**See Also** a sample TUI Script of :ref:`tui_create_plane_points` operation.


By line and point
-----------------

.. figure:: images/Plane2.png
   :align: center

   **Line and point**

A plane is created by selecting  a linear edge and point. It is possible to create a new plane perpendicular to the selected edge.

**TUI Commands**:

.. py:function:: model.addPlane(Part_doc, model.selection("EDGE", "Box_1_1/Left&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Right&Box_1_1/Bottom"), False)

    :param part: The current part object.
    :param object: A line.
    :param object: A point.
    :param boolean: Is perpendicular to line.
    :return: Result object.

Result
""""""

The Result of the operation will be a plane:

.. figure:: images/CreatedPlane2.png
   :align: center

   Plane created  

**See Also** a sample TUI Script of :ref:`tui_create_plane_line` operation.


By other plane
--------------

.. figure:: images/Plane3.png
   :align: center

   **By other plane**

A plane is created by selecting an already existing plane (planar face). There are following possibilities to define a new plane:

.. figure:: images/plane_by_distance_from_other_24x24.png
   :height: 24px
   :align: left

By distance from the selected plane.

.. figure:: images/plane_by_coincident_to_point_24x24.png
   :height: 24px
   :align: left

By coincidence with a point.

.. figure:: images/plane_by_rotation_24x24.png
   :height: 24px
   :align: left

By rotation around an edge by a specified angle.

**TUI Commands**:

.. py:function:: model.addPlane(Part_doc, model.selection("FACE", "Box_1_1/Front"), 10, False)

    :param part: The current part object.
    :param object: A plane.
    :param real: An offset.
    :param boolean: Is reverse.
    :return: Result object.

Result
""""""

The Result of the operation will be a plane parallel to already existing one:

.. figure:: images/CreatedPlane3.png
   :align: center

   **Plane parallel to a planar face**

**See Also** a sample TUI Script of :ref:`tui_create_plane_plane` operation.


By two parallel planes
----------------------
   
.. figure:: images/Plane4.png
   :align: center

   **By two parallel planes**

A plane is created by selecting two parallel planes. A new plane will be defined between them.

**TUI Commands**:

.. py:function:: model.addPlane(Part_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))

    :param part: The current part object.
    :param object: A plane 1.
    :param object: A plane 2.
    :return: Result object.

Result
""""""

The Result of the operation will be a plane created between two selected planes:

.. figure:: images/CreatedPlane4.png
   :align: center

   **Plane created between others**

**See Also** a sample TUI Script of :ref:`tui_create_plane_parallel` operation.
