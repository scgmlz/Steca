
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

#include "qcr/engine/mixin.h"
#include <QDockWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>

class QcrMainWindow : public QMainWindow, public QcrRoot {
public:
    QcrMainWindow() : QcrRoot {*this, "mainwindow"} {}
};

class QcrWidget : public QWidget, public QcrMixin {
public:
    QcrWidget(const QString& name) : QcrMixin {*this, name} {}
};

class QcrDockWidget : public QDockWidget, public QcrMixin {
public:
    QcrDockWidget(const QString& name) : QcrMixin {*this, name} {}
};

//! QLabel displaying an icon, with no associated action.
class QcrIcon : public QLabel, public QcrMixin {
public:
    QcrIcon(const QString& fileName);
};

//! Read-only QLineEdit for number display.
class QcrLineDisplay : public QLineEdit, public QcrMixin {
public:
    QcrLineDisplay() = delete;
    QcrLineDisplay(const QString& name);
    QcrLineDisplay(const QString& name, int ndigits, bool withDot);
};

#endif // VIEWS_H
