//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/dialog_save.h
//! @brief     Defines classes DialogSave, DialogMultisave
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef DIALOG_SAVE_H
#define DIALOG_SAVE_H

#include "qcr/widgets/controls.h"
#include "qcr/widgets/modal_dialogs.h"
#include <QButtonGroup>
#include <QGroupBox>
#include <QProgressBar>
#include <QTextStream>

class DialogfieldPath;


//! Base class for all export dialogs, with choice of path and format.

class DialogSave : public QcrDialog {
public:
    DialogSave() = delete;
    DialogSave(const DialogSave&) = delete;
    DialogSave(QWidget* _parent, const QString& _name, const QString& _title,
               const QStringList& _extensions);
protected:
    QProgressBar progressBar;
    QVBoxLayout* layout;
    DialogfieldPath* pathField;
    QString format() const { return saveFmt; }
    QString path() const;
    virtual void save() { saveCurrent(); }
    void saveCurrent();
    virtual void writeCurrent(QTextStream&) = 0;
    QString name2path(QString name) const;
private:
    QString saveFmt {"dat"}; //!< setting: default format for data export
};


//! Base class for multiple-file export dialogs, with choice of exportMode.

class DialogMultisave : public DialogSave {
public:
    DialogMultisave() = delete;
    DialogMultisave(const DialogMultisave&) = delete;
    DialogMultisave(QWidget* _parent, const QString& _name, const QString& _title,
                    const QStringList& _extensions, const QString& _content, const bool _haveMulti);
private:
    void save() final;
    void saveJointfile();
    void saveMultifile();
    QcrCell<int> currentSaveModeIdx {0};
    virtual int multiplicity() = 0; //!< number of files in multifile mode
    virtual void writeOnefile(QTextStream&, const int) = 0;
    virtual void writeJointfile(QTextStream&) = 0;
    QString numberedPath(const int num, const int maxNum) const;
};

#endif // DIALOG_SAVE_H
