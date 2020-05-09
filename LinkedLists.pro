TEMPLATE = subdirs

SUBDIRS += \
    LinkedListsLib \
    Utils \
    Tests \
    Applications \
    Collections

LinkedListsLib.depends = Utils
Collections.depends = LinkedListsLib
Collections.depends = Utils
Applications.depends = LinkedListsLib
Applications.depends = Utils
Tests.depends = LinkedListsLib
Tests.depends = Collections
Tests.depends = Utils
