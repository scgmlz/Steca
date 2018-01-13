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
//  class FilesModel
// ************************************************************************** //

int FilesModel::columnCount() const {
    return 2;
}

int FilesModel::rowCount() const {
    return to_i(gSession->numFiles());
}

QVariant FilesModel::data(const QModelIndex& index, int role) const {
    const int row = index.row(), rowCnt = rowCount();
    if (row < 0 || rowCnt <= row)
        return {};

    switch (role) {
    case Qt::DisplayRole:
        return gSession->file(to_u(row))->fileName();
    case GetFileRole:
        return QVariant::fromValue<shp_Datafile>(gSession->file(to_u(row)));
    default:
        return {};
    }
}

void FilesModel::removeFile(uint i) {
    gHub->removeFile(i);
}

// ************************************************************************** //
//  class MetadataModel
// ************************************************************************** //

MetadataModel::MetadataModel() {
    rowsChecked_.fill(false, Metadata::numAttributes(false));
}

void MetadataModel::reset(shp_Suite dataseq) {
    metadata_.clear();
    if (dataseq)
        metadata_ = dataseq->metadata();
    signalReset();
}

int MetadataModel::columnCount() const {
    return NUM_COLUMNS;
}

int MetadataModel::rowCount() const {
    return to_i(Metadata::numAttributes(false));
}

QVariant MetadataModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};

    int col = index.column();

    switch (role) {
    case Qt::CheckStateRole:
        switch (col) {
        case COL_CHECK: return rowsChecked_.at(to_u(row)) ? Qt::Checked : Qt::Unchecked;
        }
        break;

    case Qt::DisplayRole:
        switch (col) {
        case COL_TAG: return Metadata::attributeTag(to_u(row), false);
        case COL_VALUE: return metadata_ ? metadata_->attributeStrValue(to_u(row)) : "-";
        }
        break;
    }

    return {};
}

QVariant MetadataModel::headerData(int, Qt::Orientation, int) const {
    return {};
}

void MetadataModel::flipCheck(uint row) {
    rowsChecked_[row] = !rowsChecked_[row];
    signalReset();
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
