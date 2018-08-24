
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/views.h
//! @brief     Defines enhanced view widgets QcrMainWindow, QcrWidget, QcrLineDisplay, and others
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
#include <QScrollArea>

class QcrMainWindow : public QMainWindow, public QcrRoot {
public:
    QcrMainWindow() : QcrRoot{this} {}
};

class QcrWidget : public QWidget, public QcrMixin {
public:
    QcrWidget() : QcrMixin(this) {}
};

class QcrFrame : public QFrame, public QcrMixin {
public:
    QcrFrame() : QcrMixin{this} {}
};

class QcrScrollArea : public QScrollArea, public QcrMixin {
public:
    QcrScrollArea() : QcrMixin{this} {}
};

class QcrDockWidget : public QDockWidget, public QcrMixin {
public:
    QcrDockWidget(const QString& name) : QcrMixin{this, name} {}
};

//! QLabel displaying an icon, with no associated action.
class QcrIcon : public QLabel, public QcrMixin {
public:
    QcrIcon(const QString& fileName);
};

//! Read-only QLineEdit for number display.
class QcrLineDisplay : public QLineEdit, public QcrMixin {
public:
    QcrLineDisplay(std::function<QString()> freshText);
    QcrLineDisplay(int ndigits, bool withDot);
};

#endif // VIEWS_H
