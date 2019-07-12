.. |revolution_cut_btn.icon|    image:: images/revolution_cut_btn.png

Revolution Cut
==============

Revolution Cut feature revolves the selected objects around the selected axis and cuts the result from other objects.

To perform a Revolution Cut in the active part:

#. select in the Main Menu *Features - > Revolution Cut* item  or
#. click |revolution_cut_btn.icon| **Revolution Cut** button in the toolbar

The following property panel will be opened:

.. image:: images/StartSketch.png
  :align: center

.. centered::
  Start sketch

There are two variants of the property panel for Revolution Cut depending on the chosen option:

.. image:: images/revolution_by_angles.png
   :align: left
**By Angles** revolves objects by specifying angles.

.. image:: images/revolution_by_bounding_planes.png
   :align: left
**By Bounding Planes** revolves objects by specifying bounding planes and angles.


By angles
--------

.. image:: images/RevolutionCut1.png
  :align: center

.. centered::
  Revolution Cut: definition by angles

- **Base objects** - contains a list of objects selected in the Object Browser or in the Viewer, which will be revolved.
- **Axis** - axis of revolution.
- **To angle** - end angle of revolution.
- **From angle** - start angle of revolution.
- **Cut from** - contains a list of objects which will but cut with the result of revolution.

**TUI Commands**:

.. py:function:: model.addRevolutionCut(part, objectsToRevolve, axis, angle, objectToCut)

    :param part: The current part object.
    :param list: A list of objects for revolution.
    :param object: An axis.
    :param real: An angle.
    :param list: A list of objects to cut from.
    :return: Created object.

.. py:function:: model.addRevolutionCut(part, objectsToRevolve, axis, toAngle, fromAngle, objectToCut)

    :param part: The current part object.
    :param list: A list of objects for revolution.
    :param object: An axis.
    :param real: To angle.
    :param real: From angle.
    :param list: A list of objects to cut from.
    :return: Created object.

Result
""""""

The Result of the operation will be a revolved shape:

.. image:: images/revolution_cut_by_angles_result.png
	   :align: center

.. centered::
   **Revolution Cut created**

**See Also** a sample TUI Script of :ref:`tui_create_revolution_cut_by_angles` operation.

By bounding planes
------------------

.. image:: images/RevolutionCut2.png
  :align: center

.. centered::
  Revolution Cut: definition by bounding planes

- **Base objects** - contains a list of objects selected in the Object Browser or in the Viewer, which will be revolved.
- **Axis** - axis of revolution.
- **To plane** - a planar face can be selected to bound revolution from one side.
- **To offset** - offset for revolution or for bounding plane, if selected.
- **From plane** - a planar face can be selected to bound revolution from the other side.
- **From offset** - offset for revolution or for bounding plane, if selected.
- **Cut from** - contains a list of objects from which the result of revolution will be cut.

**TUI Command**:

.. py:function:: model.addRevolutionCut(part, objectsToRevolve, axis, toObject, toOffset, fromObject, fromOffset, objectToCut)

    :param part: The current part object.
    :param list: A list of objects for revolution.
    :param object: An axis.
    :param object: To object.
    :param real: To offset.
    :param object: From object.
    :param real: From offset.
    :param list: A list of objects to cut from.
    :return: Created object.

Result
""""""

The Result of the operation will be a revolved shape:

.. image:: images/revolution_cut_by_bounding_planes_result.png
	   :align: center

.. centered::
   **Revolution Cut created**

**See Also** a sample TUI Script of :ref:`tui_create_revolution_cut_by_bounding_planes` operation.