
Plane
=====

Plane feature creates a new constructive plane.

Plane is a construction object that can be created in a part or in a partset. To create a plane:

#. select in the Main Menu *Construction - > Plane* item  or
#. click **Point** button in the toolbar

.. image:: images/plane_button.png
  :align: center

.. centered::
  **Plane** button

There are 4 algorithms for creation of a Plane:

.. image:: images/plane_by_three_points_32x32.png
   :align: left
**By three points** creates a plane by three points.

.. image:: images/plane_by_line_and_point_32x32.png
   :align: left
**By line and point** creates a plane by a line and a point.

.. image:: images/plane_by_other_plane_32x32.png
   :align: left
**By other plane** creates a plane parallel to another plane.

.. image:: images/plane_by_two_parallel_planes_32x32.png
   :align: left
**By two parallel planes** creates a plane between two parallel planes.


By three points
---------------

.. image:: images/Plane1.png
   :align: center
	
.. centered::
   **By three points**

A plane is created by selecting three points in a viewer.

**TUI Commands**: *model.addPlane(Part_doc, model.selection("VERTEX", "Box_1_1/Back&Box_1_1/Left&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Right&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Left&Box_1_1/Bottom"))*

**Arguments**: Part + 3 vertices.

Result
""""""

The Result of the operation will be a plane:

.. image:: images/CreatedPlane1.png
	   :align: center

.. centered::
   **A plane by three points**

**See Also** a sample TUI Script of :ref:`tui_create_plane_points` operation.


By line and point
-----------------

.. image:: images/Plane2.png
   :align: center
	
.. centered::
   **Line and point**

A plane is created by selecting  a linear edge and point. It is possible to create a new plane perpendicular to the selected edge.

**TUI Commands**: *model.addPlane(Part_doc, model.selection("EDGE", "Box_1_1/Left&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Right&Box_1_1/Bottom"), False)*

**Arguments**: Part + line + point + is perpendicular to line flag.

Result
""""""

The Result of the operation will be a plane:

.. image:: images/CreatedPlane2.png
	   :align: center

.. centered::
   Plane created  

**See Also** a sample TUI Script of :ref:`tui_create_plane_line` operation.


By other plane
--------------

.. image:: images/Plane3.png
   :align: center
	
.. centered::
   **By other plane**

A plane is created by selecting an already existing plane (planar face). There are following possibilities to define a new plane:

.. image:: images/plane_by_distance_from_other_24x24.png
   :align: left

By distance from the selected plane.

.. image:: images/plane_by_coincident_to_point_24x24.png
   :align: left

By coincidence with a point.

.. image:: images/plane_by_rotation_24x24.png
   :align: left

By rotation around an edge by a specified angle.

**TUI Commands**: *model.addPlane(Part_doc, model.selection("FACE", "Box_1_1/Front"), 10, False)*

**Arguments**: Part + a plane + offset + is reverse flag.

Result
""""""

The Result of the operation will be a plane parallel to already existing one:

.. image:: images/CreatedPlane3.png
	   :align: center

.. centered::
   **Plane parallel to a planar face**

**See Also** a sample TUI Script of :ref:`tui_create_plane_plane` operation.


By two parallel planes
----------------------
   
.. image:: images/Plane4.png
   :align: center
	
.. centered::
   **By two parallel planes**

A plane is created by selecting two parallel planes. A new plane will be defined between them.

**TUI Commands**: *model.addPlane(Part_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))*

**Arguments**: Part + 2 planes.

Result
""""""

The Result of the operation will be a plane created between two selected planes:

.. image:: images/CreatedPlane4.png
	   :align: center

.. centered::
   **Plane created between others**

**See Also** a sample TUI Script of :ref:`tui_create_plane_parallel` operation.
