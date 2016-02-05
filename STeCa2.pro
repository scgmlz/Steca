TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS  = core gui
SUBDIRS += tests

gui.depends = core
tests.depends = core

OTHER_FILES = \
  README.md \
  manifest.h \
  Doxyfile
