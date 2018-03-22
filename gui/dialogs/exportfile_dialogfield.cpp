// ************************************************************************** //
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
// ************************************************************************** //

#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/dialogs/dialog_panels.h"
#include "gui/base/file_dialog.h"
#include "gui/mainwin.h"

namespace {
static QString const DAT_EXT(".dat"), DAT_SEP(" "), // extension, separator
    CSV_EXT(".csv"), CSV_SEP(", ");
static QString saveFmt = DAT_EXT; //!< setting: default format for data export
}

ExportfileDialogfield::ExportfileDialogfield(bool withTypes)
{
    static QDir defaultDir = QDir::homePath();

    auto* rbDat = new CRadioButton {"rbDat#", DAT_EXT};
    auto* rbCsv = new CRadioButton {"rbCsv#", CSV_EXT};
    auto* actBrowse = new CTrigger("actBrowse#", "Browse...");
    dir_ = new QLineEdit(defaultDir.absolutePath());
    file_ = new QLineEdit();

    rbCsv->setChecked(saveFmt == CSV_EXT);
    rbDat->setChecked(saveFmt == DAT_EXT);
    dir_->setReadOnly(true);

    connect(actBrowse, &QAction::triggered, [this]() {
        file_dialog::queryDirectory(this, "Select folder", defaultDir);
        dir_->setText(defaultDir.absolutePath()); });
    connect(rbDat, &QRadioButton::clicked, []() { saveFmt = DAT_EXT; });
    connect(rbCsv, &QRadioButton::clicked, []() { saveFmt = CSV_EXT; });

    auto* destination = new GridPanel("Destination");
    destination->grid_.addWidget(new QLabel("Save to folder:"), 0, 0, Qt::AlignRight);
    destination->grid_.addWidget(dir_, 0, 1);
    destination->grid_.addWidget(new XTextButton(actBrowse), 0, 2);
    destination->grid_.addWidget(new QLabel("File name:"), 1, 0, Qt::AlignRight);
    destination->grid_.addWidget(file_, 1, 1);

    auto* ftype = new GridPanel("File type");
    ftype->setVisible(withTypes);
    ftype->grid_.addWidget(rbDat, 0, 0);
    ftype->grid_.addWidget(rbCsv, 1, 0);

    grid_ = new QGridLayout();
    grid_->addWidget(destination, 0, 0);
    grid_->addWidget(ftype, 0, 1);
    setLayout(grid_);
}

QString ExportfileDialogfield::filePath(bool withSuffix, bool withNumber)
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

QString ExportfileDialogfield::separator() const
{
    if      (saveFmt==DAT_EXT)
        return DAT_SEP;
    else if (saveFmt==CSV_EXT)
        return CSV_SEP;
    else
        THROW("bug: invalid format");
}
