// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_table.h
//! @brief     A dynamic table for ouput data.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef OUT_TABLE_H
#define OUT_TABLE_H

#include "gui_helpers.h"
#include "panel/panel.h"

class TheHub;

//------------------------------------------------------------------------------

class OutTableModel;

class OutTable: public TreeView {
  SUPER(OutTable,TreeView)
public:
  /// The usual comparator: <0, 0, >0
  typedef int cmpFun(QVariant const&, QVariant const&);

  typedef QVector<cmpFun*>  cmp_vec;
  typedef QVector<bool>     bool_vec;

  typedef QVector<QVariant> row_t;

public:
  OutTable(uint numDataColumns);

  void setHeaders(str_lst const&);
  void setCmpFuns(cmp_vec const&);

  void clear();
  void addRow(row_t const&);

private:
  OutTableModel *model;
};

//------------------------------------------------------------------------------
/// Comparators

int cmp_int(QVariant const&,  QVariant const&);
int cmp_str(QVariant const&,  QVariant const&);
int cmp_real(QVariant const&, QVariant const&);
int cmp_date(QVariant const&, QVariant const&);

//------------------------------------------------------------------------------

class OutTableWidget: public QWidget {
  SUPER(OutTableWidget,QWidget)
public:
  // Must have the right number of headers and comparators
  OutTableWidget(TheHub&,
                 str_lst const& headers, OutTable::cmp_vec const&);
 ~OutTableWidget();

  OutTable& table() const {
    return *outTable;
  }

private:
  struct ShowItem {
    str tag; QCheckBox *cb;
  };

  typedef QVector<ShowItem> showitem_vec;

private:
  class ShowItemsWidget: public QWidget {
    SUPER(ShowItemsWidget,QWidget)
  public:
    ShowItemsWidget(showitem_vec&);
    QGridLayout *grid;
  };

private:
  TheHub          &theHub;
  OutTable        *outTable;
  ShowItemsWidget *showItemsWidget;
  showitem_vec     showItems;
};

//------------------------------------------------------------------------------
/// a child window that deletes itself

class OutWindow: public QFrame {
  SUPER(OutWindow,QFrame)
public:
  OutWindow(rcstr title,QWidget*);

protected:
  void setWidgets(panel::BasicPanel*,OutTableWidget*);

  QBoxLayout        *box;
  panel::BasicPanel *panel;
  OutTableWidget    *tableWidget;

  virtual void calculate() = 0;
};

//------------------------------------------------------------------------------
#endif // OUT_TABLE_H
