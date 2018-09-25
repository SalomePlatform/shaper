
Arc
===

The feature Arc creates an arc segment in the current Sketch.

To add new Arc to the Sketch:

#. select in the Main Menu *Sketch - > Arc* item  or
#. click **Arc** button in Sketch toolbar:

.. image:: images/arc.png
   :align: center

.. centered::
   **Arc**  button

There are 3 algorithms for creation of an Arc:

.. image:: images/arc_base_32x32.png
   :align: left
**By center and two points** creates an arc segment with the given center passing from the start point to the end point on the circumference.

.. image:: images/arc_3pt_32x32.png
   :align: left
**By three points** creates an arc segment passing from the start point to end point through another point on the circumference.

.. image:: images/arc_tang_32x32.png
   :align: left
**By tangent point and end point** creates an arc segment with the tangent point and the end point.

By center and two points
""""""""""""""""""""""""

.. image:: images/Arc_panel_base.png
   :align: center

Click in the view once to set the center point, then move the mouse and click a second time to set the start point
and finally move the mouse and click a third time to set the last passed point.

- When entering a center or start point by selecting either a point or a segment, a Coincident constraint is created.
- When entering an end point by selecting a segment, a Coincident constraint is created.
- When entering an end point, only segments are selectable.

**TUI Command**:  *Sketch_1.addArc(CenterX, CenterY, StartX, StartY, EndX, EndY, Inversed)*

**Arguments**:    7 values (coordinates of the center, the start, the end, inversed flag (if true - build arc from end to start)).

By three points
"""""""""""""""

.. image:: images/Arc_panel_3pt.png
   :align: center

Click in the view once to set the start point, then move the mouse and click a second time to set the end point
and finally move the mouse and click a third time to set the passed point.

- When entering a start or end point by selecting either a point or a segment, a Coincident constraint is created.
- When entering a passing point by selecting a point, a Coincident constraint is created.
- When entering a passing point by selecting a segment, a Tangent constraint is created.

**TUI Command**:  *Sketch_1.addArc(StartX, StartY, EndX, EndY, PassedX, PassedY)*

**Arguments**:    6 values (coordinates of the start, end and passed points).

By tangent point and point
""""""""""""""""""""""""""

.. image:: images/Arc_panel_tang.png
   :align: center

Select point on segement in the view to set the tangent point, then move the mouse and click to set the end point.
Tangent point by itself is a start point. The edge on which it lies will be tangent to the arc.

- When entering a tangent point by selecting a point on segment, a Tangent constraint is created.
- When entering an end point by selecting a segment, a Coincident constraint is created.
- When entering an end point, only segments are selectable.

**TUI Command**:  *Sketch_1.addArc(TangetPoint, EndX, EndY, Inversed)*

**Arguments**:    4 values (reference to tangent point, coordinates of end point, inversed flag (if true - build arc from end to start)).

Result
""""""

Created arc appears in the view.

.. image:: images/Arc_res.png
	   :align: center

.. centered::
   Circle created

**See Also** a sample TUI Script of a :ref:`tui_create_arc` operation.