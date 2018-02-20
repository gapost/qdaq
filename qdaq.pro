################################################################
#
# QDaq - Qt-based data aqcuisition
#
# Sub-Project Description
#
################################################################
#
# 1. qtpropertybrowser
#
# Builds the property browser from qt-solutions,
# https://github.com/qtproject/qt-solutions, as a static lib.
# The source code resides in lib/3rdparty/qt-solutions/qtpropertybrowser
#
# 2. lib
#
# Builds the QDaq shared library (libQDaq.so or QDaq.dll). All source code is in lib folder.
#
# 3. qdaq
#
# Builds the qdaq application, which can be used to run qdaq-javascript scripts.
#
# 4. designer
#
# Builds the QtDesigner plugins for some of the widgets in QDaq lib.
#
# 5. doc
#
# Dummy project, contains the doxygen config file
#
# 6. test
#
# Dummy project, contains test qdaq applications for testing and debugging.
#
###################################################################

TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS += \
    lib \
    designer \
    qdaq \
    doc \
    test

