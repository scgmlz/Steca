// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      output_dialogs.h
//! @brief     Output dialogs.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef OUTPUT_DIALOGS_H
#define OUTPUT_DIALOGS_H

#include "actions.h"
#include "calc/calc_reflection_info.h"
#include "panels/panel.h"
#include "typ/typ_log.h"

class QProgressBar;

namespace gui { namespace output {
//------------------------------------------------------------------------------

/* Note that some data members are public, to simplify the code. Be careful. */

class Panel : public panel::GridPanel {
  CLS(Panel) SUPER(panel::GridPanel)
public:
  using super::super;
};

class PanelReflection : public Panel {
  CLS(PanelReflection) SUPER(Panel)
public:
  PanelReflection(TheHub&);

  QComboBox *cbRefl;
};

class PanelGammaSlices : public Panel {
  CLS(PanelGammaSlices) SUPER(Panel)
public:
  PanelGammaSlices(TheHub&);

  QSpinBox       *numSlices;
  QDoubleSpinBox *stepGamma;

  void updateValues();
};

class PanelGammaRange : public Panel {
  CLS(PanelGammaRange) SUPER(Panel)
public:
  PanelGammaRange(TheHub&);

  QCheckBox      *cbLimitGamma;
  QDoubleSpinBox *minGamma, *maxGamma;

  void updateValues();
};

class PanelPoints : public Panel {
  CLS(PanelPoints) SUPER(Panel)
public:
  PanelPoints(TheHub&);

  QRadioButton *rbCalc, *rbInterp;
};

class PanelInterpolation : public Panel {
  CLS(PanelInterpolation) SUPER(Panel)
public:
  PanelInterpolation(TheHub&);

  QDoubleSpinBox *stepAlpha, *stepBeta, *idwRadius;
  QDoubleSpinBox *avgAlphaMax, *avgRadius; QSpinBox *avgThreshold;
};

class PanelDiagram : public Panel {
  CLS(PanelDiagram) SUPER(Panel)
public:
  PanelDiagram(TheHub&);

  QComboBox *xAxis, *yAxis;
};

class PanelFitError : public Panel {
  CLS(PanelFitError) SUPER(Panel)
public:
  PanelFitError(TheHub&);

#ifdef DEVELOP_REBECCA
  QComboBox         *cbErrorUnits;
  QRadioButton      *rbPercent, *rbAbs;
  QDoubleSpinBox    *spFitError;

times:
  *intensityFitError_, *tthFitError_, *fwhmFitError_;
  QComboBox                 *cbErrorTypes_;
#endif
};

//------------------------------------------------------------------------------

class Params : public QWidget, protected RefHub {
  CLS(Params) SUPER(QWidget)
public:
  enum ePanels {
    REFLECTION = 0x01, GAMMA = 0x02, POINTS = 0x04, INTERPOLATION = 0x08,
    DIAGRAM = 0x10,
  };

  Params(TheHub&, ePanels);
 ~Params();

  PanelReflection    *panelReflection;
  PanelGammaSlices   *panelGammaSlices;
  PanelGammaRange    *panelGammaRange;
  PanelPoints        *panelPoints;
  PanelInterpolation *panelInterpolation;
  PanelDiagram       *panelDiagram;

  str saveDir, saveFmt;

private:
  void readSettings();
  void saveSettings() const;

  QBoxLayout *box_;
};

//------------------------------------------------------------------------------

class Table : public TreeView, protected RefHub {
  CLS(Table) SUPER(TreeView)
public:
  Table(TheHub&, uint numDataColumns);

  void setColumns(str_lst::rc headers, typ::cmp_vec::rc);
  str_lst const headers();

  void clear();
  void addRow(typ::row_t::rc, bool sort);

  void sortData();

  uint rowCount() const;
  typ::row_t::rc row(uint) const;

private:
  scoped<class TableModel*> model_;
};

//------------------------------------------------------------------------------

class Tabs : public panel::TabsPanel {
  CLS(Tabs) SUPER(panel::TabsPanel)
public:
  Tabs(TheHub&);
};

//------------------------------------------------------------------------------

class Tab : public QWidget, protected RefHub {
  CLS(Tab) SUPER(QWidget)
public:
  Tab(TheHub&, Params&);

protected:
  Params &params_;

  GridLayout *grid_;
};

//------------------------------------------------------------------------------

class TabTable : public Tab {
  CLS(TabTable) SUPER(Tab)
public:
  TabTable(TheHub&, Params&, str_lst::rc headers, typ::cmp_vec::rc);

private:
  struct showcol_t {
    str        name;
    QCheckBox *cb;
  };

  typedef typ::vec<showcol_t> showcol_vec;

private:
  class ShowColsWidget : public QWidget {
    CLS(ShowColsWidget) SUPER(QWidget)
  public:
    ShowColsWidget(Table&, showcol_vec&);

  private:
    Table  &table_; showcol_vec& showCols_;

    QBoxLayout   *box_;
    QRadioButton *rbHidden_, *rbAll_, *rbNone_, *rbInten_, *rbTth_, *rbFWHM_;
  };

public:
  Table          *table;

private:
  ShowColsWidget *showColumnsWidget_;
  showcol_vec     showCols_;
};

//------------------------------------------------------------------------------

class TabSave : public Tab {
  CLS(TabSave) SUPER(Tab)
public:
  TabSave(TheHub&, Params&, bool withTypes);

  str filePath(bool withSuffix);
  str separator() const;

  Action *actBrowse, *actSave;

protected:
  str fileSetSuffix(rcstr);

  QLineEdit    *dir_, *file_;
  QRadioButton *rbDat_, *rbCsv_;
};

//------------------------------------------------------------------------------

class Frame : public QFrame, protected RefHub {
  CLS(Frame) SUPER(QFrame)
public:
  Frame(TheHub&, rcstr title, Params*, QWidget*);

protected:
  QAction     *actClose_, *actCalculate_, *actInterpolate_;
  QToolButton *btnClose_, *btnCalculate_, *btnInterpolate_;

  QProgressBar *pb_;

  QBoxLayout *box_;
  Params     *params_;
  Tabs       *tabs_;

  typ::vec<calc::ReflectionInfos> calcPoints_, interpPoints_;

  Table *table_;

  void calculate();
  void interpolate();

  virtual void displayReflection(int reflIndex, bool interpolated);

protected:
  int  getReflIndex()    const;
  bool getInterpolated() const;
};

//------------------------------------------------------------------------------
}}
#endif // OUTPUT_DIALOGS_H
