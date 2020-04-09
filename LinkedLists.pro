TEMPLATE = subdirs

SUBDIRS += \
    Application \
    LinkedListsLib

Application.depends = LinkedListsLib
