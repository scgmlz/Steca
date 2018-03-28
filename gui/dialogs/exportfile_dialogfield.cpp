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

#include "core/typ/exception.h"
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/base/file_dialog.h"
#include <QGroupBox>

namespace {
static QString const DAT_EXT(".dat"), DAT_SEP(" "), // extension, separator
    CSV_EXT(".csv"), CSV_SEP(", ");
static QString saveFmt = DAT_EXT; //!< setting: default format for data export
}

ExportfileDialogfield::ExportfileDialogfield(
    QWidget* parent, bool withTypes, std::function<void(void)> onSave)
{
    progressBar.hide();

    static QDir defaultDir = QDir::homePath();

    dir_ = new QLineEdit(defaultDir.absolutePath());
    file_ = new QLineEdit();
    auto* rbDat = new CRadioButton("fmtDat", DAT_EXT);
    auto* rbCsv = new CRadioButton("fmtCsv", CSV_EXT);
    auto* actBrowse = new CTrigger("selectDir", "Browse...");
    auto* actCancel = new CTrigger("cancel", "Cancel");
    auto* actSave = new CTrigger("save", "Save");

    rbCsv->setChecked(saveFmt == CSV_EXT);
    rbDat->setChecked(saveFmt == DAT_EXT);
    dir_->setReadOnly(true);

    // internal connections
    connect(actBrowse, &QAction::triggered, [this, parent]() {
        file_dialog::queryDirectory(parent, "Select folder", defaultDir);
        dir_->setText(defaultDir.absolutePath()); });
    connect(rbDat, &QRadioButton::clicked, []() { saveFmt = DAT_EXT; });
    connect(rbCsv, &QRadioButton::clicked, []() { saveFmt = CSV_EXT; });

    // outgoing connections
    connect(actCancel, &QAction::triggered, [parent]() { parent->close(); });
    connect(actSave, &QAction::triggered, onSave);

    // layout
    auto* destinationGrid = new QGridLayout;
    destinationGrid->addWidget(new QLabel("Save to folder:"), 0, 0, Qt::AlignRight);
    destinationGrid->addWidget(dir_,                          0, 1);
    destinationGrid->addWidget(new CTextButton(actBrowse),    0, 2);
    destinationGrid->addWidget(new QLabel("File name:"),      1, 0, Qt::AlignRight);
    destinationGrid->addWidget(file_,                         1, 1);

    auto* destination = new QGroupBox("Destination");
    destination->setLayout(destinationGrid);

    auto* ftypeGrid = new QVBoxLayout;
    ftypeGrid->addWidget(rbDat);
    ftypeGrid->addWidget(rbCsv);

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
    bottom->addWidget(new CTextButton(actCancel));
    bottom->addWidget(new CTextButton(actSave));

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
    return file_dialog::openFileConfirmOverwrite("file", parentWidget(), tmp);
}

QString ExportfileDialogfield::separator() const
{
    if      (saveFmt==DAT_EXT)
        return DAT_SEP;
    else if (saveFmt==CSV_EXT)
        return CSV_SEP;
    else
        THROW("bug: invalid format");
}
