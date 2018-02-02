// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/tab_save.h
//! @brief     Defines class TabSave
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TAB_SAVE_H
#define TAB_SAVE_H

#include "gui/base/new_q.h"

//! Base class for dialogs for saving some output to a file.

class TabSave : public QWidget {
public:
    TabSave(bool withTypes);
    str filePath(bool withSuffix, bool withNumber=false);
    str separator() const;
    QAction *actBrowse, *actSave;
protected:
    QGridLayout* grid_;
private:
    QLineEdit *dir_, *file_;
    QRadioButton *rbDat_, *rbCsv_;
};

#endif // TAB_SAVE_H
