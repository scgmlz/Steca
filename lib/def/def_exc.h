/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

// exceptions

#ifndef DEF_EXC_H
#define DEF_EXC_H

#include "def_macros.h"
#include "typ/typ_str.h"
#include <QException>
//------------------------------------------------------------------------------

// An exception that carries a message.
class Exception : public QException {
  CLS(Exception) SUPER(QException)
protected:
  Exception(rcstr msg, bool silent) noexcept;
public:

  Exception()           noexcept;
  Exception(rcstr msg)  noexcept;
  Exception(rc)         noexcept;

  bool  silent() const noexcept { return silent_; }
  rcstr msg()    const noexcept { return msg_;    }
  pcstr what()   const noexcept;

  void setMsg(rcstr);

  Exception* clone() const;
  void       raise() const;

protected:
  str        msg_;
  QByteArray msg8bit_;
  bool       silent_;
};

// raise an exception
#define THROW(msg)     throw Exception(msg)
#define THROW_SILENT() throw Exception()

// run-time condition checking
#define RUNTIME_CHECK(test, msg) \
  if (!(test)) THROW(msg)

//------------------------------------------------------------------------------
#endif // DEF_EXC_H
// eof
