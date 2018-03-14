// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/cfg/settings.h
//! @brief     Defines class Settings
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QAction>
#include <QSettings>
#include <QSpinBox>

//! Used to save and retrieve the main window geometry, and certain parameter settings.

//! TODO: make a clear decision, what is saved through this mechanism, and what is
//! left to the .ste configuration files.
class Settings : public QSettings {
public:
    Settings(const QString& group = "");
    ~Settings();

    QVariant readVariant(const QString& key, const QVariant& def);
    void saveVariant(const QString& key, const QVariant& val) { setValue(key, val); }

    void read(const QString& key, QAction*, bool def = false);
    void save(const QString& key, QAction*);

    void read(const QString& key, QSpinBox*, int def = 0);
    void save(const QString& key, QSpinBox*);

    void read(const QString& key, QDoubleSpinBox*, qreal def = 0);
    void save(const QString& key, QDoubleSpinBox*);

    bool readBool(const QString& key, bool def = false) { return readVariant(key, def).toBool(); }
    void saveBool(const QString& key, bool val) { saveVariant(key, val); }

    int readInt(const QString& key, int def = 0);
    void saveInt(const QString& key, int val) { saveVariant(key, val); }

    qreal readReal(const QString& key, qreal def = 0);
    void saveReal(const QString& key, qreal val) { saveVariant(key, val); }

    QString readStr(const QString& key, const QString& def = "") {
        return readVariant(key, def).toString(); }
    void saveStr(const QString& key, const QString& val) { saveVariant(key, val); }
};

#endif // SETTINGS_H
