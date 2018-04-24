//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/exportfile_dialogfield.cpp
//! @brief     Implements classes ExportfileDialogfield
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "exportfile_dialogfield.h"
#include "core/typ/exception.h"
#include "gui/dialogs/file_dialog.h"
#include "qcr/engine/debug.h"
#include <QGroupBox>

namespace {
static QString const
    DAT_EXT(".dat"), DAT_SEP(" "), // extension, separator
    CSV_EXT(".csv"), CSV_SEP(", ");
static QString saveFmt = DAT_EXT; //!< setting: default format for data export
}


ExportfileDialogfield::eFileOverridePolicy ExportfileDialogfield::fileOverridePolicy = ExportfileDialogfield::eFileOverridePolicy::PROMT;

ExportfileDialogfield::ExportfileDialogfield(
    QWidget* parent, bool withTypes, std::function<void(void)> onSave)
{
    progressBar.hide();

    static QDir defaultDir = QDir::homePath();

    dir_ = new QcrLineEdit("dir", defaultDir.absolutePath());
    file_ = new QcrLineEdit("file");

    rbCsv_.programaticallySetValue(saveFmt == CSV_EXT);
    rbDat_.programaticallySetValue(saveFmt == DAT_EXT);
    dir_->setReadOnly(true);

    // internal connections
    connect(&actBrowse_, &QAction::triggered, [this, parent]() {
            dir_->setText(file_dialog::queryDirectory(parent, "Select folder", dir_->text())); });
    connect(&rbDat_, &QRadioButton::clicked, []() { saveFmt = DAT_EXT; });
    connect(&rbCsv_, &QRadioButton::clicked, []() { saveFmt = CSV_EXT; });

    // outgoing connections
    connect(&actCancel_, &QAction::triggered, [parent]() { parent->close(); });
    connect(&actSave_, &QAction::triggered, onSave);

    // layout
    auto* destinationGrid = new QGridLayout;
    destinationGrid->addWidget(new QLabel("Save to folder:"), 0, 0, Qt::AlignRight);
    destinationGrid->addWidget(dir_,                          0, 1);
    destinationGrid->addWidget(new QcrTextButton(&actBrowse_),0, 2);
    destinationGrid->addWidget(new QLabel("File name:"),      1, 0, Qt::AlignRight);
    destinationGrid->addWidget(file_,                         1, 1);

    auto* destination = new QGroupBox("Destination");
    destination->setLayout(destinationGrid);

    auto* ftypeGrid = new QVBoxLayout;
    ftypeGrid->addWidget(&rbDat_);
    ftypeGrid->addWidget(&rbCsv_);

    auto* ftype = new QGroupBox("File type");
    ftype->setVisible(withTypes);
    ftype->setLayout(ftypeGrid);

    auto* setup = new QHBoxLayout;
    setup->addWidget(destination);
    setup->addWidget(ftype);

    auto* bottom = new QHBoxLayout();
    bottom->addWidget(&progressBar);
    bottom->setStretchFactor(&progressBar, 333);
    bottom->addStretch(1);
    bottom->addWidget(new QcrTextButton(&actCancel_));
    bottom->addWidget(new QcrTextButton(&actSave_));

    addLayout(setup);
    addLayout(bottom);
}

QString ExportfileDialogfield::path(bool withSuffix, bool withNumber)
{
    QString dir = dir_->text().trimmed();
    QString fileName = file_->text().trimmed();
    if (dir.isEmpty() || fileName.isEmpty())
        return "";
    if (withNumber && !fileName.contains("%d"))
        fileName += ".%d";
    if (withSuffix) {
        QString suffix = saveFmt;
        if ("."+QFileInfo(fileName).suffix().toLower()!=saveFmt.toLower())
            fileName += saveFmt;
    }
    file_->setText(fileName);

    return QFileInfo(dir + '/' + fileName).absoluteFilePath();
}

QFile* ExportfileDialogfield::file()
{
    QString tmp = path(true);
    if (tmp.isEmpty())
        return {};
    switch (fileOverridePolicy) {
    case eFileOverridePolicy::PROMT:
        return file_dialog::openFileConfirmOverwrite("file", parentWidget(), tmp);
        break;
    case eFileOverridePolicy::PANIC:
        THROW("attempting to write to already existing file: '" + tmp + "'");
        break;
    case eFileOverridePolicy::SILENT_OVERRIDE:
        return file_dialog::openFileForcedOverwrite("file", parentWidget(), tmp);
        break;
    default:
        THROW("Unexpected fileOverridePolicy state: "
              + QString::fromStdString(std::to_string((int)fileOverridePolicy)));
        break;
    }


}

QString ExportfileDialogfield::separator() const
{
    if      (saveFmt==DAT_EXT)
        return DAT_SEP;
    else if (saveFmt==CSV_EXT)
        return CSV_SEP;
    else
        qFatal("invalid case in ExportfileDialogfield::separator");
}
