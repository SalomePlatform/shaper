
Python addons
=============

User can create his own custom features.

Corresponding folder should be created for each feature at *../sources/src/PythonAddons/macros*.

Feature description includes 4 files:

- widget.xml with description of property panel,
-  __init__.py,
- feature.py with python commands, 
- icon.png with image of button in toolbar (file is located at sub-folder /icons).

Two examples of custom features already created are:

.. toctree::
   :titlesonly:
   :maxdepth: 1

   boxFeature.rst
   rectangleFeature.rst
