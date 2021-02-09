TEMPLATE = subdirs

SUBDIRS += \
    ManualListEntry

unix {
    SUBDIRS += \
        BoundedBuffer \
        UnnamedPipes \
        NamedPipes \
        MessageQueues \
        Sockets \
        SharedFile \
        ASyncTest \
        FuturePromiseTest
}

unix:!macx {
    SUBDIRS += \
        SharedMemory \
        Signals
}
