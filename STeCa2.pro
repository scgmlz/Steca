TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = core gui

gui.depends =
core

OTHER_FILES = \
  README.md \
  manifest.h
