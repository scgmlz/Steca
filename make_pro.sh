# usage: make_pro.sh <extra CONFIG>

PRO=STeCa2.pro
echo -e '# generated project\n' > $PRO

APP=STeCa2_`date +%y.%m.%d_%H%M`

cat >> $PRO <<EOT
TARGET   = $APP
TEMPLATE = app

QT      += core gui widgets svg
CONFIG  += c++11 silent

win32 {
  CONFIG += static
  RC_ICONS = \$\$PWD/gui/icons/STeCa2.ico
}

INCLUDEPATH += \$\$PWD/core \$\$PWD/gui

EOT

function files {
  where=$1; ext=$2
  find $where -type f -name \*.$ext -exec echo ' ' {} \\ \;
}

echo -e '\nHEADERS += \\'	>> $PRO
files core h			>> $PRO
files gui  h			>> $PRO

echo -e '\nSOURCES += \\'	>> $PRO
files core cpp			>> $PRO
files gui  cpp			>> $PRO

echo -e '\nRESOURCES += \\'	>> $PRO
files core qrc			>> $PRO
files gui  qrc			>> $PRO

cat >> $PRO <<EOT

OTHER_FILES += \\
    .gitignore \\
    make_pro.sh ours_wc.sh \\
    README.md CODING.md \\
    COPYING Doxyfile \\
    manifest.h \\
    TODO
EOT
