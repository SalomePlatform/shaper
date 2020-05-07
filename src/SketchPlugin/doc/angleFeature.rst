.. |angle_constr.icon|    image:: images/angle_constr.png

Angle constraint
================

Angle constraint fixes the angle between two lines.

| Angle between two lines is seen as an angle between two vectors.
| Each line is treated as a vector with start and end points equal to those of the line.

To create an Angle constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Angle* item  or
#. click |angle_constr.icon| **Angle** button in Sketch toolbar:

Property panel:

.. figure:: images/Angle_panel.png
   :align: center

.. |angle_direct| image:: images/angle_direct.png
.. |angle_complement| image:: images/angle_complementary.png
.. |angle_backward| image:: images/angle_backward.png
.. |loc_left| image:: images/location_left.png
.. |loc_auto| image:: images/location_automatic.png
.. |loc_right| image:: images/location_right.png

Input fields:

- **Line 1** is the first line selected in the view;
- **Line 2** is the second line selected in the view;
- **Value** is an angle between the lines, can be modified to set the desirable value;
- **Angle type** is a type of angle measurement:
   | |angle_direct| **Direct** is the least angle between two lines;
   | |angle_complement| **Complementary** is 180°- Direct angle between two lines;
   | |angle_backward| **Backward** is 360°- Direct angle between two lines.
- **Text location** is the position of the angle value label relatively to the angle line (in the view):
   | |loc_left| **Left** inserts text to the left of the angle line;
   | |loc_auto| **Automatic** inserts text it the middle of the angle line if it has enough length, otherwise - to the left;
   | |loc_right| **Right** inserts text to the right of the angle line.

When both lines are selected, the angle value is displayed in the property panel and in the view.

When creating the constraint, after selection of two lines at the first time:

- drag the angle presentation in the view to the desired position and click once;
- set desirable angle value in the input field in the view and press **Enter** or just press **Enter** to keep the current angle

.. figure:: images/Angle_field_view.png
   :align: center

   Angle input in the view

**TUI Commands**:

.. py:function:: Sketch_1.setAngle(Line1, Line2, Value)
.. py:function:: Sketch_1.setAngleComplementary(Line1, Line2, Value)
.. py:function:: Sketch_1.setAngleBackward(Line1, Line2, Value)

    :param object: Line 1.
    :param object: Line 2.
    :param real: Value.
    :return: Result object.

Result
""""""

Created Angle appears in the view.

.. figure:: images/Angle_res.png
   :align: center

   Angle created

**See Also** a sample TUI Script of :ref:`tui_create_angle` operation.
