TEMPLATE = subdirs

SUBDIRS += \
    LinkedListsLib \
    Utils \
    Tests \
    Applications \
    Collections

Applications.depends = LinkedListsLib
Applications.depends = Utils
Collections.depends = LinkedListsLib
Collections.depends = Utils
LinkedListsLib.depends = Utils
Tests.depends = LinkedListsLib
Tests.depends = Collections
Tests.depends = Utils
