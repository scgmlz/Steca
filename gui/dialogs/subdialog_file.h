//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/subdialog_file.h
//! @brief     Defines class DialogfieldFile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SUBDIALOG_FILE_H
#define SUBDIALOG_FILE_H

#include "qcr/widgets/controls.h"
#include "qcr/widgets/modal_dialogs.h"
#include <QButtonGroup>
#include <QGroupBox>
#include <QProgressBar>

class DialogfieldPath;

//! Common part of all export dialogs: choice of file and format.

class DialogfieldFile : public QVBoxLayout {
public:
    DialogfieldFile() = delete;
    DialogfieldFile(const DialogfieldFile&) = delete;
    DialogfieldFile(
        QcrDialog* _parent, QStringList extensions,
        std::function<void(QFile* file, const QString& format, QcrDialog* parent)> _onSave);

    QString format() const { return saveFmt; }
    DialogfieldPath* pathField;
    QProgressBar progressBar;
    QString path(bool withSuffix, bool withNumber);
private:
    QcrDialog* parent;
    QString saveFmt {"dat"}; //!< setting: default format for data export
    std::function<void(QFile* file, const QString& format, QcrDialog* parent)> onSave;
};

//! Export dialog for multiple files.

class DialogfieldMultifile : public DialogfieldFile {
public:
    DialogfieldMultifile() = delete;
    DialogfieldMultifile(const DialogfieldMultifile&) = delete;
    DialogfieldMultifile(
        QcrDialog* _parent, QStringList _extensions,
        std::function<void(QFile* file, const QString& format, QcrDialog* parent)> _onSave,
        const QString& _content, const bool _haveMulti);
    enum class ExportMode {
        CURRENT_PEAK = 0,
        ALL_PEAKS_MULTIPLE_FILES = 1,
        ALL_PEAKS_SINGLE_FILE = 2,
    };
    ExportMode exportMode() const { return (ExportMode)(currentSaveModeIdx.val()); }
private:
    QcrCell<int> currentSaveModeIdx {0};
};


#endif // SUBDIALOG_FILE_H
