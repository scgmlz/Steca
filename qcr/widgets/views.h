
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

class QcrMainWindow : public QMainWindow, public QcrRootMixin {
public:
    QcrMainWindow() : QcrRootMixin{this} {}
};

class QcrWidget : public QWidget, public QcrBaseMixin {
public:
    QcrWidget() = delete;
    QcrWidget(const QString& name) : QcrBaseMixin(this, name) {}
};

class QcrFrame : public QFrame, public QcrBaseMixin {
public:
    QcrFrame() = delete;
    QcrFrame(const QString& name) : QcrBaseMixin{this, name} {}
};

class QcrScrollArea : public QScrollArea, public QcrBaseMixin {
public:
    QcrScrollArea() = delete;
    QcrScrollArea(const QString& name) : QcrBaseMixin{this, name} {}
};

class QcrDockWidget : public QDockWidget, public QcrBaseMixin {
public:
    QcrDockWidget(const QString& name) : QcrBaseMixin{this, name} {}
};

//! QLabel displaying an icon, with no associated action.
class QcrIcon : public QLabel, public QcrBaseMixin {
public:
    QcrIcon(const QString& fileName);
};

//! Read-only QLineEdit for number display.
class QcrLineDisplay : public QLineEdit, public QcrBaseMixin {
public:
    QcrLineDisplay(std::function<QString()> freshText);
    QcrLineDisplay(int ndigits, bool withDot);
};

#endif // VIEWS_H
