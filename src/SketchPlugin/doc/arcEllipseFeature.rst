.. |earc.icon|    image:: images/elliptic_arc.png

Elliptic Arc
============

Elliptic Arc feature creates an elliptic arc segment in the current Sketch.

To add a new Elliptic Arc to the Sketch:

#. select in the Main Menu *Sketch - > Elliptic Arc* item  or
#. click |earc.icon| **Elliptic Arc** button in Sketch toolbar:

The following property panel appears:

.. image:: images/elliptic_arc_panel.png
   :align: center

Click in the view to set the center point, then move the mouse and click a second time to set a point of a one semi-axis, then click to set a starting point of the arc and then to set an end point of the arc.

**TUI Command**:

.. py:function:: SketchEllipticArc_1 = Sketch_1.addEllipticArc(X1, Y1, X2, Y2, X3, Y3, X4, Y4, False)
.. py:function:: [SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")

    :param real: Center X.
    :param real: Center Y.
    :param real: Start X.
    :param real: Start Y.
    :param real: End X.
    :param real: End Y.
    :param boolean: Is inversed.
    :return: Result object.

Result
""""""

Created elliptic arc appears in the view.

.. image:: images/elliptic_arc_result.png
	   :align: center

.. centered::
   Elliptic arc created

**See Also** a sample TUI Script of :ref:`tui_create_elliptic_arc` operation.