wc $(find . \( -name \*.h -or -name \*.cpp \) ! \( -path ./LevMar/\* -or -path ./gui/QCP/\* -or -path ./tests/\* \) | sort)
