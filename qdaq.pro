################################################################
#
# QDaq - Qt-based data aqcuisition
#
# Sub-Project Description
#
################################################################
#
# 1. core
#
# Builds the QDaqCore shared library (libQDaqCore.so or QDaqCore.dll). 
#
# 2. gui
#
#
# 3. qdaq
#
# Builds the qdaq application, which can be used to run qdaq-javascript scripts.
#
# 3. designer
#
# Builds the QtDesigner plugins for some of the widgets in QDaqGui.
#
# 4. docs
#
# Dummy project, contains the doxygen config file and documentation
#
# 5. test
#
# Dummy project, contains test qdaq applications for testing and debugging.
#
# 6. plugins
#
# Project to build qdaq plugins
#
###################################################################

TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS += \
    core \
    gui \
    qdaq \
    designer \
    filters \
    interfaces \
    docs \
    test \
    plugins
#    plugins \


qdaqspec.files  = qdaq.pri qdaq.prf
qdaqspec.path  = $$[QMAKE_MKSPECS]/features

INSTALLS += qdaqspec

