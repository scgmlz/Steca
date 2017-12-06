# usage: make_pro.sh <extra CONFIG>

PRO=STeCa2.pro
echo -e '# generated project\n' > $PRO

APP=STeCa2-`date +%y%m%d_%H%M`

cat >> $PRO <<EOT
TARGET   = $APP
TEMPLATE = app

QT      += 3rdparty core gui widgets svg network
CONFIG  += c++11 silent

win32 {
  CONFIG += static
  RC_ICONS = \$\$PWD/gui/icons/retro_stier.ico
}

INCLUDEPATH += \$\$PWD/3rdparty/LevMar \$\$PWD/3rdparty/QCustomPlot \$\$PWD/lib \$\$PWD/io \$\$PWD/core \$\$PWD/gui

EOT

function files {
  where=$1; ext=$2
  find $where -type f -name \*.$ext -exec echo ' ' {} \\ \;
}

MODULES='lib LevMar io core gui'
echo -e '\nHEADERS += \\' >> $PRO
for m in $MODULES ; do files $m h >> $PRO ; done

echo -e '\nSOURCES += \\' >> $PRO
for m in $MODULES ; do files $m cpp >> $PRO ; done

echo -e '\nRESOURCES += \\' >> $PRO
files gui  qrc >> $PRO

cat >> $PRO <<EOT

OTHER_FILES += \\
  .gitignore \\
  make_pro.sh ours_wc.sh \\
  README.md CODING.md \\
  COPYING Doxyfile \\
  manifest.h \\
  TODO
EOT
