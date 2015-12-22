#ifndef LAYOUT_HELPERS_H
#define LAYOUT_HELPERS_H

#include "defs.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

QHBoxLayout* hbox();
QVBoxLayout* vbox();
QGridLayout* grid();

int textWidth(rcstr);

#endif // LAYOUT_HELPERS_H
