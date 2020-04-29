.. |intersection.icon|    image:: images/Intersection.png

Intersection
============

| The Intersection operation creates an intersection of a 3D edge and the sketch plane.
| The result of the intersection is a point.

To create an Intersection in the active Sketch:

#. select in the Main Menu *Sketch - > Intersection* item  or
#. click |intersection.icon| **Intersection** button in Sketch toolbar:

Property panel:

.. image:: images/Intersection_panel.png
  :align: center

.. centered::
   Intersection

Input fields:

- **Object** is the 3D edge to intersect with the sketch plane.
- **Include into the sketch result** option defines whether to include the intersection point into the sketch result.

**TUI Commands**:

.. py:function:: Sketch_1.addIntersectionPoint(Edge, IncludeIntoResult)*

    :param object: An edge.
    :param boolean: Include into the result flag.
    :return: Result object.

.. py:function:: Sketch_1.addIntersectionPoint(EdgeName, IncludeIntoResult)*

    :param object: An edge name.
    :param boolean: Include into the result flag.
    :return: Result object.

Result
""""""

Created Intersection appears in the view.

.. image:: images/Intersection_res.png
	   :align: center

.. centered::
   Created intersection (purple point)

**See Also** a sample TUI Script of :ref:`tui_create_intersection` operation.
