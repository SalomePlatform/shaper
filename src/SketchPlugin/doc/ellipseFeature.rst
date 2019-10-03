.. |ellipse.icon|    image:: images/ellipse.png

Ellipse
=======

The feature Ellipse creates an ellipse in the current Sketch.

To add a new Ellipse to the Sketch:

#. select in the Main Menu *Sketch - > Ellipse* item  or
#. click |ellipse.icon| **Ellipse** button in Sketch toolbar:

There are 2 algorithms for creation of an Ellipse:

.. image:: images/ellipse_cent_rad_32x32.png
   :align: left
**By center and major semi-axes and passing points** creates an ellipse with the given center passing through given points.

.. image:: images/ellipse_axes_32x32.png
   :align: left
**By major axis and passing point** creates an ellipse passing through the given three points.

By center and major semi-axes and passing points
""""""""""""""""""""""""""""""""""""""""""""""""

.. image:: images/ellipse_panel_pt_rad.png
   :align: center

Click in the view once to set the center point, then move the mouse and click to set a point of a first semi-axis, then again move the mouse to set a point of a second semi-axis.

**TUI Command**:

.. py:function:: SketchEllipse_1 = Sketch_1.addEllipse(X1, Y1, X2, Y2, X3)
.. py:function:: [SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")

    :param real: Start X.
    :param real: Start Y.
    :param real: Passed X.
    :param real: Passed Y.
    :return: Result object.

By major axis and passing point
"""""""""""""""""""""""""""""""

.. image:: images/ellipse_panel_3pt.png
   :align: center

Click in the view once to set a first point of a first axis, then move the mouse and click to set a second point of the first axis, then again move the mouse to set a point of a second semi-axis.

**TUI Command**:

.. py:function:: SketchEllipse_1 = Sketch_1.addEllipse(X1, Y1, X2, Y2, X3)
.. py:function:: [SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")

    :param real: Start X.
    :param real: Start Y.
    :param real: Passed X.
    :param real: Passed Y.
    :return: Result object.

Result
""""""

Created ellipse appears in the view.

.. image:: images/ellipse_result.png
	   :align: center

.. centered::
   Ellipse created


**See Also** a sample TUI Script of :ref:`tui_create_ellipse` operation.
