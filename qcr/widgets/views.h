
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

class QcrMainWindow : virtual public QcrRoot, virtual public QMainWindow {
public:
    QcrMainWindow() {}
};

class QcrWidget : virtual public QcrBase, virtual public QWidget {
public:
    QcrWidget() = delete;
    QcrWidget(const QString& name) : QcrBase(name) {}
};

class QcrFrame : virtual public QcrBase, virtual public QFrame {
public:
    QcrFrame() = delete;
    QcrFrame(const QString& name) : QcrBase{name} {}
};

class QcrScrollArea : virtual public QcrBase, virtual public QScrollArea {
public:
    QcrScrollArea() = delete;
    QcrScrollArea(const QString& name) : QcrBase{name} {}
};

class QcrDockWidget : virtual public QcrBase, virtual public QDockWidget {
public:
    QcrDockWidget(const QString& name) : QcrBase{name} {}
};

//! QLabel displaying an icon, with no associated action.
class QcrIcon : public QLabel, public QcrBase {
public:
    QcrIcon(const QString& fileName);
};

//! Read-only QLineEdit for number display.
class QcrLineDisplay : virtual public QcrBase, virtual public QLineEdit {
public:
    QcrLineDisplay(std::function<QString()> freshText);
    QcrLineDisplay(int ndigits, bool withDot);
};

#endif // VIEWS_H
