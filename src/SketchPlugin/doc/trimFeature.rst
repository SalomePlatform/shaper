.. |trim.icon|    image:: images/Trim.png

Trim
====

| The Trim operation trims away the specified segment of the curve.
| The curve should have points lying on it or intersections with other curves.
| Open curves (line or arc) require one or more points; closed curves (circle) require two or more points.

To create a Trim in the active Sketch:

#. select in the Main Menu *Sketch - > Trim* item  or
#. click |trim.icon| **Trim** button in Sketch toolbar:

Property panel:

.. image:: images/Trim_panel.png
  :align: center

.. centered::
   Trim

Input fields:

- **Segment** is used to select a segment to remove in the view.

The selected segment is highlighted in the view:

.. image:: images/Trim_segment_sel.png
   :align: center

.. centered::
   The segment to remove

**TUI Command**:

.. py:function:: Sketch_1.addTrim(Feature, PositionPoint)

    :param object: Line, arc or circle.
    :param object: Position point.
    :return: Result object.

**Arguments**:   feature (line, arc or circle) and position point (a point on or closest to the segment to remove)

Result
""""""

Created Trim appears in the view.

.. image:: images/Trim_res.png
	   :align: center

.. centered::
   Created trim

**See Also** a sample TUI Script of :ref:`tui_create_trim` operation.
