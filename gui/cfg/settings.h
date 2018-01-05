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

#include "typ/str.h"
#include <QSettings>

class QAction;
class QDoubleSpinBox;
class QSpinBox;



class Settings : public QSettings {
private:
public:
    Settings(rcstr group = "");
    ~Settings();

    QVariant readVariant(rcstr key, QVariant const& def);
    void saveVariant(rcstr key, QVariant const& val) { setValue(key, val); }

    void read(rcstr key, QAction*, bool def = false);
    void save(rcstr key, QAction*);

    void read(rcstr key, QSpinBox*, int def = 0);
    void save(rcstr key, QSpinBox*);

    void read(rcstr key, QDoubleSpinBox*, qreal def = 0);
    void save(rcstr key, QDoubleSpinBox*);

    bool readBool(rcstr key, bool def = false) { return readVariant(key, def).toBool(); }
    void saveBool(rcstr key, bool val) { saveVariant(key, val); }

    int readInt(rcstr key, int def = 0);
    void saveInt(rcstr key, int val) { saveVariant(key, val); }

    qreal readReal(rcstr key, qreal def = 0);
    void saveReal(rcstr key, qreal val) { saveVariant(key, val); }

    str readStr(rcstr key, rcstr def = "") { return readVariant(key, def).toString(); }
    void saveStr(rcstr key, rcstr val) { saveVariant(key, val); }
};



#endif // SETTINGS_H
