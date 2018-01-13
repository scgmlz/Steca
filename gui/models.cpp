// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/models.cpp
//! @brief     Implements classes TableModel, FilesModel, MeasurementsModel, MetadataModel, ReflectionsModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/models.h"
#include "core/fit/fit_fun.h"
#include "core/session.h"
#include "gui/thehub.h"

// ************************************************************************** //
//  class TableModel
// ************************************************************************** //

void TableModel::signalReset() {
    beginResetModel();
    endResetModel();
}

// ************************************************************************** //
//  class ReflectionsModel
// ************************************************************************** //

ReflectionsModel::ReflectionsModel() : TableModel() {}

int ReflectionsModel::columnCount() const {
    return NUM_COLUMNS;
}

int ReflectionsModel::rowCount() const {
    return to_i(gSession->reflections().count());
}

str ReflectionsModel::displayData(uint row, uint col) const {
    switch (col) {
    case COL_ID:
        return str::number(row + 1);
    case COL_TYPE:
        return gSession->reflections().at(row)->peakFunction().name();
    default:
        NEVER return "";
    }
}

str ReflectionsModel::displayData(uint row) const {
    return displayData(row, COL_ID) + ": " + displayData(row, COL_TYPE);
}

QVariant ReflectionsModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};

    switch (role) {
    case Qt::DisplayRole: {
        int col = index.column();
        if (col < 1)
            return {};

        switch (col) {
        case COL_ID:
        case COL_TYPE:
            return displayData(to_u(row), to_u(col));
        default:
            return {};
        }
    }

    case GetMeasurementRole:
        return QVariant::fromValue<shp_Reflection>(gSession->reflections().at(to_u(row)));
    default:
        return {};
    }
}

QVariant ReflectionsModel::headerData(int col, Qt::Orientation, int role) const {
    if (Qt::DisplayRole == role && COL_ID == col)
        return "#";
    else
        return {};
}

void ReflectionsModel::addReflection(const QString& peakFunctionName) {
    gHub->addReflection(peakFunctionName);
}

void ReflectionsModel::remReflection(uint i) {
    gHub->remReflection(i);
}
