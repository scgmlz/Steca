
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

#include "qcr/engine/cell.h"
#include <QMainWindow>

class WidgetCell : public Cell {
public:
    WidgetCell(class QcrMixin* qwner, const QString& name) : Cell(name), qwner_(qwner)  {}
private:
    class QcrMixin* qwner_;
    void recompute() final;
};

class QcrMixin {
public:
    QcrMixin(QObject* owner, const QString& name) : owner_(owner), defaultCell {this, name} {}
    void remake();
    Cell* cell() { return cell_; }
private:
    Cell* cell_ {&defaultCell};
    QObject* owner_;
    WidgetCell defaultCell;
};

class QcrWidget : public QWidget, public QcrMixin {
public:
    QcrWidget(const QString& name) : QcrMixin {this, name} {}
};

class QcrMainWindow : public QMainWindow, public QcrMixin {
public:
    QcrMainWindow() : QcrMixin {this, "mainwindow"} {}
};

#endif // VIEWS_H
