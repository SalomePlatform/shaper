
Extrusion Cut
=============

Extrusion Cut feature extrudes selected objects along their normals or selected axis and cuts result from other objects.

To perform Extrusion Cut in the active part:

#. select in the Main Menu *Features - > Extrusion Cut* item  or
#. click **Extrusion Cut** button in the toolbar

.. image:: images/extrusion_cut_btn.png
   :align: center

.. centered::
   **Extrusion Cut** button

The following property panel will be opened:

.. image:: images/StartSketch.png
  :align: center

.. centered::
  Start sketch

After selecting one of the options property panel for Extrusion Cut will be opened with two variants:

.. image:: images/extrusion_by_sizes.png
   :align: left
**By Sizes** extrudes objects by specifying sizes.

.. image:: images/extrusion_by_bounding_planes.png
   :align: left
**By Bounding Planes** extrudes objects by specifying bounding planes and offsets.


By sizes
--------

.. image:: images/ExtrusionCut1.png
  :align: center

.. centered::
  Extrusion Cut: definition by sizes

- **Base objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be extruded.
- **Axis** if selected then it will be direction of extrusion, otherwise objects normals will be used.
- **To size** size for extrusion along direction.
- **From size** size for extrusion in opposite direction.
- **Cut from** contains a list of objects which will but cut with the result of extrusion.

**TUI Command**:  *model.addExtrusionCut(part, objectsToExtrude, size, objectsToCut);*

**Arguments**:   Part + list of objects for extrusion + size + list of objects to cut from.

**TUI Command**:  *model.addExtrusionCut(part, objects, direction, size, objectsToCut);*

**Arguments**:   Part + list of objects for extrusion + direction + size + list of objects to cut from.

**TUI Command**:  *model.addExtrusionCut(part, objects, toSize, fromSize, objectsToCut);*

**Arguments**:   Part + list of objects for extrusion + to size + from size + list of objects to cut from.

**TUI Command**:  *model.addExtrusionCut(part, objects, direction, toSize, fromSize, objectsToCut);*

**Arguments**:   Part + list of objects for extrusion + direction + to size + from size + list of objects to cut from.

Result
""""""

The Result of the operation will be an extruded shape:

.. image:: images/extrusion_cut_by_sizes_result.png
	   :align: center

.. centered::
   **Extrusion Cut created**

**See Also** a sample TUI Script of a :ref:`tui_create_extrusion_cut_by_sizes` operation.

By bounding planes
------------------

.. image:: images/ExtrusionCut2.png
  :align: center

.. centered::
  Extrusion Cut: definition by bounding planes

- **Base objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be extruded.
- **Axis** if selected then it will be direction of extrusion, otherwise objects normals will be used.
- **To plane** planar face can be selected to bound extrusion from one side.
- **To offset** offset for extrusion or for bounding plane if selected.
- **From plane** planar face can be selected to bound extrusion from other side.
- **From offset** offset for extrusion or for bounding plane if selected.
- **Cut from** contains a list of objects which will but cut with the result of extrusion.

**TUI Command**:  *model.addExtrusionCut(part, objects, toObject, toOffset, fromObject, fromOffset, objectsToCut);*

**Arguments**:   Part + list of objects for extrusion + to object + to offset + from object + from offset + list of objects to cut from.

**TUI Command**:  *model.addExtrusionCut(part, objects, direction, toObject, toOffset, fromObject, fromOffset, objectsToCut);*

**Arguments**:   Part + list of objects for extrusion + direction + to object + to offset + from object + from offset + list of objects to cut from.

Result
""""""

The Result of the operation will be an extruded shape:

.. image:: images/extrusion_cut_by_bounding_planes_result.png
	   :align: center

.. centered::
   **Extrusion Cut created**

**See Also** a sample TUI Script of a :ref:`tui_create_extrusion_cut_by_bounding_planes` operation.
