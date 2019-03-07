//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/tables.h
//! @brief     Defines classes TableModel, CheckTableModel and TableView, CheckTableView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef TABLES_H
#define TABLES_H

#include "qcr/engine/mixin.h"
#include <QTreeView>

//! Pure virtual base class of all models of rectangular table form

class TableModel : public QAbstractTableModel {
public:
    TableModel() = delete;
    TableModel(const QString& name);
    TableModel(const TableModel&) = delete;

    void refreshModel(); // within rectangle plus one row
    virtual void onClicked(const QModelIndex& cell);
    void setHighlightedCell(const QModelIndex& cell);

    int columnCount(const QModelIndex& /*unused*/) const { return columnCount(); }
    int rowCount(const QModelIndex& /*unused*/) const { return rowCount(); }
    const QString& name() const { return name_; }
    void setName(const QString &name) { name_ = name; }

    virtual int columnCount() const = 0;
    virtual int rowCount() const = 0;
    virtual int highlighted() const = 0;
    virtual void onHighlight(int i) = 0;
private:
    QString name_;
    int rowCountCached_ {-1};
};

//! Pure virtual base class for rectangular table models with rows that can be checked.

class CheckTableModel : public TableModel {
public:
    CheckTableModel(const QString& name);

    QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant entry(int row, int col) const { return {}; }
    virtual QString tooltip(int row, int col) const { return ""; }
    virtual QColor foregroundColor(int row, int col) const { return QColor(Qt::black); }

    void onActivated();
    void onClicked(const QModelIndex& cell) override;
    virtual bool activated(int row) const = 0;
    virtual Qt::CheckState state(int row) const {
        return activated(row) ? Qt::Checked : Qt::Unchecked; }
    virtual void setActivated(int row, bool on) = 0;
    void activateAndLog(int row, bool on);
};

//! Pure virtual base class of all views of rectangular table form

//! Based on QTreeView, with hidden 1st column.
//! QTreeView inherits from QAbstractItemView.

class TableView : public QcrRegistered, public QTreeView {
public:
    TableView(TableModel*);
    ~TableView();

    void setFromCommand(const QString&) override;
    virtual void onData();
protected:
    int dWidth() const;
    int mWidth() const;
    QString name_;
    TableModel* model_;
    void currentChanged(const QModelIndex& current, const QModelIndex&) override;
    void gotoCurrent(const QModelIndex&);
    void updateScroll();
    void highlight(int row);
};

//! Pure virtual base class for rectangular table models with rows that can be checked.

class CheckTableView : public TableView {
public:
    CheckTableView(TableModel* model);
    void setFromCommand(const QString&) override;
private:
    CheckTableModel* model() { return static_cast<CheckTableModel*>(model_); }
};

#endif // TABLES_H
