
Extrusion Fuse
=============

Extrusion Fuse feature extrudes selected objects along their normals or selected axis and fuses result with other objects.

To perform Extrusion Fuse in the active part:

#. select in the Main Menu *Features - > Extrusion Fuse* item  or
#. click **Extrusion Fuse** button in the toolbar

.. image:: images/extrusion_fuse_btn.png
   :align: center

.. centered::
   **Extrusion Fuse** button

The following property panel will be opened:

.. image:: images/StartSketch.png
  :align: center

.. centered::
  Start sketch

After selecting one of the options property panel for Extrusion Fuse will be opened with two variants:

.. image:: images/extrusion_by_sizes.png
   :align: left
**By Sizes** extrudes objects by specifying sizes.

.. image:: images/extrusion_by_bounding_planes.png
   :align: left
**By Bounding Planes** extrudes objects by specifying bounding planes and offsets.


By sizes
--------

.. image:: images/ExtrusionFuse1.png
  :align: center

.. centered::
  Extrusion Fuse: definition by sizes

- **Base objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be extruded.
- **Axis** if selected then it will be direction of extrusion, otherwise objects normals will be used.
- **To size** size for extrusion along direction.
- **From size** size for extrusion in opposite direction.
- **Fuse with** contains a list of objects which will but fused with the result of extrusion.

**TUI Command**:  *model.addExtrusionFuse(part, objectsToExtrude, size, objectsToFuse);*

**Arguments**:   Part + list of objects for extrusion + size + list of objects to fuse with.

**TUI Command**:  *model.addExtrusionFuse(part, objects, direction, size, objectsToFuse);*

**Arguments**:   Part + list of objects for extrusion + direction + size + list of objects to fuse with.

**TUI Command**:  *model.addExtrusionFuse(part, objects, toSize, fromSize, objectsToFuse);*

**Arguments**:   Part + list of objects for extrusion + to size + from size + list of objects to fuse with.

**TUI Command**:  *model.addExtrusionFuse(part, objects, direction, toSize, fromSize, objectsToFuse);*

**Arguments**:   Part + list of objects for extrusion + direction + to size + from size + list of objects to fuse with.

Result
""""""

The Result of the operation will be an extruded shape:

.. image:: images/extrusion_fuse_by_sizes_result.png
	   :align: center

.. centered::
   **Extrusion Fuse created**

**See Also** a sample TUI Script of a :ref:`tui_create_extrusion_fuse_by_sizes` operation.

By bounding planes
------------------

.. image:: images/ExtrusionFuse2.png
  :align: center

.. centered::
  Extrusion Fuse: definition by bounding planes

- **Base objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be extruded.
- **Axis** if selected then it will be direction of extrusion, otherwise objects normals will be used.
- **To plane** planar face can be selected to bound extrusion from one side.
- **To offset** offset for extrusion or for bounding plane if selected.
- **From plane** planar face can be selected to bound extrusion from other side.
- **From offset** offset for extrusion or for bounding plane if selected.
- **Fuse with** contains a list of objects which will but fused with the result of extrusion.

**TUI Command**:  *model.addExtrusionFuse(part, objects, toObject, toOffset, fromObject, fromOffset, objectsToFuse);*

**Arguments**:   Part + list of objects for extrusion + to object + to offset + from object + from offset + list of objects to fuse with.

**TUI Command**:  *model.addExtrusionFuse(part, objects, direction, toObject, toOffset, fromObject, fromOffset, objectsToFuse);*

**Arguments**:   Part + list of objects for extrusion + direction + to object + to offset + from object + from offset + list of objects to fuse with.

Result
""""""

The Result of the operation will be an extruded shape:

.. image:: images/extrusion_fuse_by_bounding_planes_result.png
	   :align: center

.. centered::
   **Extrusion Fuse created**

**See Also** a sample TUI Script of a :ref:`tui_create_extrusion_fuse_by_bounding_planes` operation.
