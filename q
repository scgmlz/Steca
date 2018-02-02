#!/bin/sh
find . \( -name \*.h -or -name \*.cpp \) -and \( -path ./core/\* -or -path ./gui/\* -or -path ./main/\* \) | sort
