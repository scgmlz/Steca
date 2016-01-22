#include "core_defs.h"

static_assert (QT_VERSION >= 0x050501,"written for Qt >= 5.5");

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

str const NULL_STR;

namespace core {

interval_t::interval_t(qreal low_, qreal hig_): low(low_), hig(hig_) {
}

void interval_t::set(qreal val) {
  set(val,val);
}

void interval_t::set(qreal low_, qreal hig_) {
  low = low_; hig = hig_;
}

void interval_t::safeSet(qreal v1, qreal v2) {
  if (v1 < v2)
    set(v1,v2);
  else
    set(v2,v1);
}

void interval_t::include(qreal val) {
  low = qMin(low,val); hig = qMax(hig,val);
}

}

// eof
