//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/subdialog_file.cpp
//! @brief     Implements classes ExportfileDialogfield
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/subdialog_file.h"
#include "gui/dialogs/file_dialog.h"
//#include "qcr/base/debug.h"
#include <QGroupBox>

ExportfileDialogfield::ExportfileDialogfield(
    QcrDialog* parent, QStringList extensions,
    std::function<void(QFile* file, const QString& format, QcrDialog* parent)> onSave)
{
    progressBar.hide();

    static QDir defaultDir = QDir::homePath();

    // Widgets

    dir_ = new QcrLineEdit("dir", defaultDir.absolutePath());
    dir_->setReadOnly(true);

    file_ = new QcrLineEdit("file");

    auto* fileExtensionGroup = new QButtonGroup;
    auto* ftypeGrid = new QVBoxLayout;
    for (const QString fmt: extensions) {
        auto* rb = new QcrRadioButton{"fmt."+fmt, "."+fmt};
        rb->programaticallySetValue(saveFmt == fmt);
        connect(rb, &QRadioButton::clicked, [this,fmt]() { saveFmt = fmt; });
        fileExtensionGroup->addButton(rb);
        ftypeGrid->addWidget(rb);
    }

    auto* actBrowse_ = new QcrTrigger{"selectDir", "Browse..."};
    auto* actCancel_ = new QcrTrigger{"cancel", "Cancel"};
    auto* actSave_   = new QcrTrigger{"save", "Save"};

    connect(actBrowse_, &QAction::triggered, [this, parent]() {
            dir_->programaticallySetValue(
                file_dialog::queryDirectory(parent, "Select folder", dir_->text())); });
    connect(actCancel_, &QAction::triggered, [parent]() { parent->close(); });
    connect(actSave_, &QAction::triggered,
            [this,parent,onSave]()->void{ onSave(file(), format(), parent); parent->close(); });

    auto updateSaveable = [this,actSave_](const QString) {
                              qDebug() << "DEBUG  updateSaveable " << path(true)
                                       << " -> enabled=" << !path(true).isEmpty();
                              actSave_->setEnabled(!path(true).isEmpty());
                          };
    updateSaveable("");
    dir_ ->setHook(updateSaveable);
    file_->setHook(updateSaveable);

    // Layout

    auto* destinationGrid = new QGridLayout;
    destinationGrid->addWidget(new QLabel("Save to folder:"),       0, 0, Qt::AlignRight);
    destinationGrid->addWidget(dir_,                                0, 1);
    destinationGrid->addWidget(new QcrTextTriggerButton(actBrowse_),0, 2);
    destinationGrid->addWidget(new QLabel("File name:"),            1, 0, Qt::AlignRight);
    destinationGrid->addWidget(file_,                               1, 1);

    auto* destination = new QGroupBox("Destination");
    destination->setLayout(destinationGrid);

    auto* ftype = new QGroupBox("File type");
    ftype->setVisible(extensions.size()>1);
    ftype->setLayout(ftypeGrid);

    auto* setup = new QHBoxLayout;
    setup->addWidget(destination);
    setup->addWidget(ftype);

    auto* bottom = new QHBoxLayout();
    bottom->addWidget(&progressBar);
    bottom->setStretchFactor(&progressBar, 333);
    bottom->addStretch(1);
    bottom->addWidget(new QcrTextTriggerButton(actCancel_));
    bottom->addWidget(new QcrTextTriggerButton(actSave_));

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
        if (QFileInfo(fileName).suffix().toLower()!=saveFmt.toLower())
            fileName += "."+saveFmt;
    }
    qDebug() << "set file name " << fileName;

    return QFileInfo(dir + '/' + fileName).absoluteFilePath();
}

QFile* ExportfileDialogfield::file()
{
    QString tmp = path(true);
    if (tmp.isEmpty())
        return {};
    return file_dialog::openFileConfirmOverwrite("file", parentWidget(), tmp);
}
