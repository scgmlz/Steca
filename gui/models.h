// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/models.h
//! @brief     Defines classes TableModel, FilesModel, MeasurementsModel, MetadataModel, ReflectionsModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MODELS_H
#define MODELS_H

#include "core/data/experiment.h"
#include "core/typ/str.h"
#include "core/typ/vec.h"
#include <QAbstractTableModel>

//! The base class of all models of rectangular table form

class TableModel : public QAbstractTableModel {
public:
    void signalReset(); //!< force-emits reset() signal
    int columnCount(const QModelIndex& /*unused*/) const {
        return columnCount(); }
    int rowCount(const QModelIndex& /*unused*/) const {
        return rowCount(); }
    virtual int columnCount() const = 0;
    virtual int rowCount() const = 0;
};


#endif // MODELS_H
