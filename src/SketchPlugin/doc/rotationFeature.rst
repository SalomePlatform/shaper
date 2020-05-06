.. _sketchAngularCopy:
.. |rotate.icon|    image:: images/rotate.png

Angular copy
============

Angular copy operation creates one or multiple copies of the sketch entities by rotation relative to a specified center point.

To create an Angular copy in the active Sketch:

#. select in the Main Menu *Sketch - > Angular copy* item  or
#. click |rotate.icon| **Angular copy** button in Sketch toolbar:

Property panel:

.. figure:: images/Rotation_panel.png
   :align: center

   Angular copy

.. |full_angle| image:: images/angle_up_full_32x32.png
.. |single_angle| image:: images/angle_up_32x32.png

Input fields:

- **Segments** is the list of segments (lines, circles, arcs) selected in the view;
- **Center of rotation** is the center point selected in the view;
- **Full angle/Single angle** option:
   | |full_angle| **Full angle**: angle step of rotation is equal to the defined angle divided by the number of copies (total number minus one).
   | |single_angle| **Single angle**: angle step of rotation is equal to the defined angle.
- **Angle** is the angle of rotation.
- **Reversed** defines rotation direction. If checked - in a clockwise direction, otherwise - in a counterclockwise direction.
- **Total number of objects** is the total number of objects including the original one.


**TUI Command**:

.. py:function:: Sketch_1.addRotation(Objects, CenterPoint, Angle, NumberOfObjects, FullValue, Reversed)

    :param list: A list of objects.
    :param object: Center point.
    :param real: Angle.
    :param integer: Number of objects.
    :param boolean: Full value flag.
    :param boolean: Reversed flag.
    :return: Result object.

Result
""""""

Created Angular copy appears in the view.

| The original and an angular copy objects are marked with a special sign.
| Copy objects are drawn with a thinner line.

.. figure:: images/Rotation_res.png
   :align: center

   Created angular copy

**See Also** a sample TUI Script of :ref:`tui_create_rotation` operation.
