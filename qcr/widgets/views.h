
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/views.h
//! @brief     Defines extensions of Qt widgets
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef VIEWS_H
#define VIEWS_H

#include "qcr/engine/settable.h"
#include <QMainWindow>
#include <QDockWidget>

class QcrWidget : public QWidget, public QcrMixin {
public:
    QcrWidget(const QString& name) : QcrMixin {*this, name} {}
};

class QcrMainWindow : public QMainWindow, public QcrMixin {
public:
    QcrMainWindow() : QcrMixin {*this, "mainwindow"} {}
};

class QcrDockWidget : public QDockWidget, public QcrMixin {
public:
    QcrDockWidget(const QString& name) : QcrMixin {*this, name} { setObjectName(name); }
};

#endif // VIEWS_H
