
Revolution Fuse
===============

Revolution Fuse feature revolves the selected objects around the selected axis and fuses the result with other objects.

To perform Revolution Fuse in the active part:

#. select in the Main Menu *Features - > Revolution Fuse* item  or
#. click **Revolution Fuse** button in the toolbar

.. image:: images/revolution_fuse_btn.png
   :align: center

.. centered::
   **Revolution Fuse** button

The following property panel will be opened:

.. image:: images/StartSketch.png
  :align: center

.. centered::
  Start sketch
  
There are two variants of the property panel for Revolution Fuse depending on the chosen option:

.. image:: images/revolution_by_angles.png
   :align: left
**By Angles** revolves objects by specifying angles.

.. image:: images/revolution_by_bounding_planes.png
   :align: left
**By Bounding Planes** revolves objects by specifying bounding planes and angles.


By angles
--------

.. image:: images/RevolutionFuse1.png
  :align: center

.. centered::
  Revolution Fuse: definition by angles

- **Base objects** - contains a list of objects selected in the Object Browser or in the Viewer, which will be revolved.
- **Axis** - axis of revolution.
- **To angle** - end angle of revolution.
- **From angle**-  start angle of revolution.
- **Fuse with** contains a list of objects which will be fused with the result of revolution.

**TUI Command**:  *model.addRevolutionFuse(part, objectsToRevolve, axis, angle, objectToFuse);*

**Arguments**:   Part + list of objects for revolution + axis + angle + list of objects to fuse with.

**TUI Command**:  *model.addRevolutionFuse(part, objectsToRevolve, axis, toAngle, fromAngle, objectToFuse);*

**Arguments**:   Part + list of objects for revolution + axis + to angle + from angle + list of objects to fuse with.

Result
""""""

The Result of the operation will be a revolved shape:

.. image:: images/revolution_fuse_by_angles_result.png
	   :align: center

.. centered::
   **Revolution Fuse created**

**See Also** a sample TUI Script of :ref:`tui_create_revolution_fuse_by_angles` operation.

By bounding planes
------------------

.. image:: images/RevolutionFuse2.png
  :align: center

.. centered::
  Revolution Fuse: definition by bounding planes

- **Base objects** - contains a list of objects selected in the Object Browser or in the Viewer, which will be revolved.
- **Axis** - axis of revolution.
- **To plane** - a planar face can be selected to bound revolution from one side.
- **To offset** - offset for revolution or for bounding plane, if selected.
- **From plane** - a planar face can be selected to bound revolution from other side.
- **From offset** - offset for revolution or for bounding plane, if selected.
- **Fuse with** - contains a list of objects which will be fused with the result of revolution.

**TUI Command**:  *model.addRevolutionFuse(part, objectsToRevolve, axis, toObject, toOffset, fromObject, fromOffset, objectToFuse);*

**Arguments**:   Part + list of objects for revolution + axis + to object + to offset + from object + from offset + list of objects to fuse with.

Result
""""""

The Result of the operation will be a revolved shape:

.. image:: images/revolution_fuse_by_bounding_planes_result.png
	   :align: center

.. centered::
   **Revolution Fuse created**

**See Also** a sample TUI Script of :ref:`tui_create_revolution_fuse_by_bounding_planes` operation.