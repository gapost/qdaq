TEMPLATE = subdirs


unix: SUBDIRS +=  linux-gpib
win32: SUBDIRS +=  ni-gpib

SUBDIRS += \
    filters \
    interfaces

