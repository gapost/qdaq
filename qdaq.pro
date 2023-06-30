################################################################
#
# QDaq - Qt-based data aqcuisition
#
# Sub-Project Description
#
################################################################
#
# 1. src
#
# Contains sub-projects that build the QDaq libraries
#
# 2. plugins
#
# Contains sub-projects that build QtDesigner and QtScript plugins
#
# 3. tools
#
# 4. docs
#
# Dummy project, contains the doxygen config file and documentation
#
# 5. test
#
# Dummy project, contains test qdaq scripts for testing and debugging.
#
#
###################################################################

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS += \
    src  \
    plugins \
    tools \
    docs \
    test 

qdaqspec.files  = \
    distfiles/qdaq-core.prf \
    distfiles/qdaq-gui.prf \
    distfiles/qdaq-filters.prf \
    distfiles/qdaq-interfaces.prf

qdaqspec.path  = $$[QMAKE_MKSPECS]/features

INSTALLS += qdaqspec


