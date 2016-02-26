/** \file
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "core_defs.h"
#include <QSettings>
#include <QKeySequence>

class QAction;
class QSpinBox;
class QDoubleSpinBox;

//------------------------------------------------------------------------------

class Settings: public QSettings {
  SUPER(Settings, QSettings)
public:
  Settings(rcstr group = "");
 ~Settings();

  QVariant readVariant(rcstr key, QVariant const& def);
  void     saveVariant(rcstr key, QVariant const& val);

  void read(rcstr key, QAction*, bool def);
  void save(rcstr key, QAction*);

  void read(rcstr key, QSpinBox*, int def);
  void save(rcstr key, QSpinBox*);

  void read(rcstr key, QDoubleSpinBox*, qreal def);
  void save(rcstr key, QDoubleSpinBox*);
};

//------------------------------------------------------------------------------

class Keys {
public:
  Keys();

  typedef QKeySequence QKey;

  QKey
    keyAddFiles, keyDeleteFile, keyLoadCorrectionFile,
    keyViewStatusbar, keyRotateImage
  #ifndef Q_OS_OSX
    , keyFullscreen
  #endif
    ;
};

//------------------------------------------------------------------------------
#endif
