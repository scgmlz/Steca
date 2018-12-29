#!/bin/sh
find . \( -name \*.h -or -name \*.cpp \) -and \( -path ./qcr/\* -or -path ./ldl/\* -or -path ./core/\* -or -path ./gui/\* -or -path ./main/\* \) | sort
