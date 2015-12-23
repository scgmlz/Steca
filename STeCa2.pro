TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = core gui

gui.depends = core

OTHER_FILES =
    manifest.h
