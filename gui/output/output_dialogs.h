// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      output_dialogs.h
//! @brief     Output dialogs.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef OUTPUT_DIALOGS_H
#define OUTPUT_DIALOGS_H

#include "actions.h"
#include "core_reflection_info.h"
#include "gui_helpers.h"
#include "panels/panel.h"
#include "refhub.h"
#include "types/core_defs.h"
#include "types/core_type_variant.h"

class QProgressBar;

namespace gui { namespace output {
//------------------------------------------------------------------------------

class Params : public QWidget, protected RefHub {
  SUPER(Params, QWidget)
public:
  Params(TheHub&);
 ~Params();

  void addStretch();

  uint currReflIndex() const;
  bool interpolate()   const;

protected:
  QBoxLayout *box_;

protected:
  panel::GridPanel *gpRefl_;
public:
  QComboBox        *cbRefl;
  QRadioButton     *rbCalc, *rbInterp;

protected:
  panel::GridPanel *gpGamma_;
public:
  QDoubleSpinBox   *stepGamma;
  QCheckBox        *cbLimitGamma;
  QDoubleSpinBox   *limitGammaMin, *limitGammaMax;

protected:
  panel::GridPanel *gpInterpolation_;
public:
  QDoubleSpinBox   *stepAlpha, *stepBeta;
  QDoubleSpinBox   *avgAlphaMax, *avgRadius, *idwRadius;
  QSpinBox         *threshold;
};

//------------------------------------------------------------------------------

class Tabs : public panel::TabsPanel {
  SUPER(Tabs, panel::TabsPanel)
public:
  Tabs(TheHub&);
};

//------------------------------------------------------------------------------

class Tab : public QWidget, protected RefHub {
  SUPER(Tab, QWidget)
public:
  Tab(TheHub&, Params&);

protected:
  QGridLayout *grid_;

  Params &params_;
};

//------------------------------------------------------------------------------

class Frame : public QFrame, protected RefHub {
  SUPER(Frame, QFrame)
public:
  Frame(TheHub&, rcstr title, Params*, QWidget*);

protected:
  QAction     *actClose_, *actCalculate_, *actInterpolate_;
  QToolButton *btnClose_, *btnCalculate_, *btnInterpolate_;

  QProgressBar *pb_;

  QBoxLayout *box_;
  Params     *params_;
  Tabs       *tabs_;

  QVector<core::ReflectionInfos> calcPoints_, interpPoints_;

  class Table *table_;

  void calculate();
  void interpolate();

  virtual void displayReflection(uint reflIndex, bool interpolated);
};

//------------------------------------------------------------------------------

class Table : public TreeView, protected RefHub {
  SUPER(Table, TreeView)
public:
  Table(TheHub&, uint numDataColumns);

  void setColumns(str_lst const& headers, core::cmp_vec const&);
  str_lst const headers();

  void clear();
  void addRow(core::row_t const&, bool sort);

  void sortData();

  uint rowCount() const;
  core::row_t const& row(uint) const;

private:
  QScopedPointer<class TableModel> model_;
};

//------------------------------------------------------------------------------

class TabTable : public Tab {
  SUPER(TabTable, Tab)
public:
  TabTable(TheHub&, Params&, str_lst const &headers, core::cmp_vec const&);

  Table *table() const { return table_; }

private:
  struct showcol_t {
    str        name;
    QCheckBox *cb;
  };

  typedef QVector<showcol_t> showcol_vec;

private:
  class ShowColsWidget : public QWidget {
    SUPER(ShowColsWidget, QWidget)
  public:
    ShowColsWidget(Table&, showcol_vec&);

  private:
    Table  &table_; showcol_vec& showCols_;

    QBoxLayout   *box_;
    QRadioButton *rbHidden_, *rbAll_, *rbNone_, *rbInten_, *rbTth_, *rbFWHM_;
  };

private:
  Table          *table_;
  ShowColsWidget *showColumnsWidget_;
  showcol_vec     showCols_;
};

//------------------------------------------------------------------------------

class TabSave : public Tab {
  SUPER(TabSave, Tab)
public:
  TabSave(TheHub&, Params&);

  void clearFilename();

  str path()        const { return path_->text();     }
  str fileName()    const { return fileName_->text(); }

  Action* actSave() const { return actSave_; }

protected:
  QLineEdit *path_, *fileName_;
  Action    *actBrowsePath_, *actSave_;
};

//------------------------------------------------------------------------------
}}
#endif  // OUTPUT_DIALOGS_H
