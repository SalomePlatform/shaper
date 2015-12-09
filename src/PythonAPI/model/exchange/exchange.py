"""Import Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addImport(part, *args):
    """Add an Import feature to the Part.

    .. function:: addImport(part, file_path)

    Args:
        part (ModelAPI_Document): part document
        file_path (string): path to the imported file

    Returns:
        Import: import object
    """
    assert(args)
    feature = part.addFeature("Import")
    return Import(feature, *args)


class Import(Interface):
    """Interface class for Import feature.

    .. function:: Import(feature)

        Create interface for the feature without initialization.

    .. function:: Import(feature, file_path)

        Create interface for the feature and initialize the feature with arguments.
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Import")

        self._file_path = self._feature.data().string("file_path")

        assert(self._file_path)

        if not args:
            return

        assert(len(args) == 1)
        self.setFilePath(args[0])

        self.execute()
        pass

    def setFilePath(self, file_path):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._file_path, file_path)
        pass


def exportToFile(part, *args):
    """Perform export from the Part to file.

    .. function:: exportToFile(part, file_path, file_format, selection_list)

    Args:
        part (ModelAPI_Document): part document
        file_path (string): path to the exported file
        file_format (string): format of to the exported file
        selection_list (list of Selection): objects to export

    Returns:
        Export: export object
    """
    assert(args)
    feature = part.addFeature("Export")
    return Export(feature, *args)


class Export(Interface):
    """Interface class for Export feature.

    .. function:: Export(feature)

        Create interface for the feature without initialization.

    .. function:: Export(feature, file_path, file_format, selection_list)

        Create interface for the feature and initialize the feature with arguments.
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Export")

        self._file_path = self._feature.data().string("file_path")
        self._file_format = self._feature.data().string("file_format")
        self._objects = self._feature.data().selectionList("selection_list")

        assert(self._file_path)
        assert(self._file_format)
        assert(self._objects)

        if not args:
            return

        assert(len(args) == 3)
        self.setFilePath(args[0])
        self.setFileFormat(args[1])
        self.setObjects(args[2])

        self.execute()
        pass

    def setFilePath(self, file_path):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._file_path, file_path)
        pass

    def setFileFormat(self, file_format):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._file_format, file_format)
        pass

    def setObjects(self, objects):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._objects, object)
        pass
