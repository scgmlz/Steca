// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/tab_save.h
//! @brief     Defines class TabSave
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

// TODO rm this along with old output dialogs

#ifndef TAB_SAVE_H
#define TAB_SAVE_H

#include "gui/base/controls.h"

//! Base class for dialogs for saving some output to a file.

class TabSave : public QWidget {
public:
    TabSave(bool withTypes);
    QString filePath(bool withSuffix, bool withNumber=false);
    QString separator() const;
protected:
    QGridLayout* grid_;
private:
    QLineEdit* dir_;
    QLineEdit* file_;
};

#endif // TAB_SAVE_H
