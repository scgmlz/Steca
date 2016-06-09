wc $(find . \( -name \*.h -or -name \*.cpp \) ! \( -path ./core/io/\*/\* -or -path ./core/LevMar/\* -or -path ./gui/QCP/\* -or -path ./tests/\* \) | sort)
