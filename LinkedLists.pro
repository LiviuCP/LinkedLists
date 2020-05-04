TEMPLATE = subdirs

SUBDIRS += \
    Application \
    BoundedBuffer \
    LinkedListsLib \
    Utils \
    Tests

Application.depends = LinkedListsLib
Application.depends = Utils
LinkedListsLib.depends = Utils
Tests.depends = LinkedListsLib
Tests.depends = Utils
