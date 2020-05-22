################################################################
#
# QDaq - Qt-based data aqcuisition
#
# Sub-Project Description
#
################################################################
#
# 1. lib
#
# Builds the QDaq shared library (libQDaq.so or QDaq.dll). All source code is in lib folder.
#
# 2. qdaq
#
# Builds the qdaq application, which can be used to run qdaq-javascript scripts.
#
# 3. designer
#
# Builds the QtDesigner plugins for some of the widgets in QDaq lib.
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
    lib \
    designer \
    qdaq \
    docs \
    test \
    plugins

