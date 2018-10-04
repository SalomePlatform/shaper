
Extrusion
=========

Extrusion feature extrudes selected objects along their normals or selected axis.

To perform Extrusion in the active part:

#. select in the Main Menu *Features - > Extrusion* item  or
#. click **Extrusion** button in the toolbar

.. image:: images/extrusion_btn.png
   :align: center

.. centered::
   **Extrusion** button

The following property panel will be opened:

.. image:: images/StartSketch.png
  :align: center

.. centered::
  Start sketch

After selecting one of the options property panel for Extrusion will be opened with two variants:

.. image:: images/extrusion_by_sizes.png
   :align: left
**By Sizes** extrudes objects by specifying sizes.

.. image:: images/extrusion_by_bounding_planes.png
   :align: left
**By Bounding Planes** extrudes objects by specifying bounding planes and offsets.


By sizes
--------

.. image:: images/Extrusion1.png
  :align: center

.. centered::
  Extrusion: definition by sizes

- **Base objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be extruded.
- **Axis** if selected then it will be direction of extrusion, otherwise objects normals will be used.
- **To size** size for extrusion along direction.
- **From size** size for extrusion in opposite direction.

**TUI Command**:  *model.addExtrusion(part, objects, size);*

**Arguments**:   Part + list of objects + size.

**TUI Command**:  *model.addExtrusion(part, objects, direction, size);*

**Arguments**:   Part + list of objects + direction + size.

**TUI Command**:  *model.addExtrusion(part, objects, toSize, fromSize);*

**Arguments**:   Part + list of objects + to size + from size.

**TUI Command**:  *model.addExtrusion(part, objects, direction, toSize, fromSize);*

**Arguments**:   Part + list of objects + direction + to size + from size.

Result
""""""

The Result of the operation will be an extruded shape:

.. image:: images/extrusion_by_sizes_result.png
	   :align: center

.. centered::
   **Extrusion created**

**See Also** a sample TUI Script of a :ref:`tui_create_extrusion_by_sizes` operation.

By bounding planes
------------------

.. image:: images/Extrusion2.png
  :align: center

.. centered::
  Extrusion: definition by bounding planes

- **Base objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be extruded.
- **Axis** if selected then it will be direction of extrusion, otherwise objects normals will be used.
- **To plane** planar face can be selected to bound extrusion from one side.
- **To offset** offset for extrusion or for bounding plane if selected.
- **From plane** planar face can be selected to bound extrusion from other side.
- **From offset** offset for extrusion or for bounding plane if selected.

**TUI Command**:  *model.addExtrusion(part, objects, toObject, toOffset, fromObject, fromOffset);*

**Arguments**:   Part + list of objects + to object + to offset + from object + from offset.

**TUI Command**:  *model.addExtrusion(part, objects, direction, toObject, toOffset, fromObject, fromOffset);*

**Arguments**:   Part + list of objects + direction + to object + to offset + from object + from offset.

Result
""""""

The Result of the operation will be an extruded shape:

.. image:: images/extrusion_by_bounding_planes_result.png
	   :align: center

.. centered::
   **Extrusion created**

**See Also** a sample TUI Script of a :ref:`tui_create_extrusion_by_bounding_planes` operation.
