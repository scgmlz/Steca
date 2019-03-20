//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/console.cpp
//! @brief     Implements class Console
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/engine/logger.h"
#include "qcr/base/debug.h"
#include <QApplication>
#include <QFile>

namespace {
QTextStream qterr{stderr};
} // namespace

Logger* gLogger; //!< global

Logger::Logger(const QString& logFileName)
{
    gLogger = this;

    auto* file = new QFile{logFileName};
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        qFatal("cannot open log file %s for writing", CSTRI(logFileName));
    log_.setDevice(file);
    startTime_ = QDateTime::currentDateTime();
    setCaller("log");
    log("# " + qApp->applicationName() + " " + qApp->applicationVersion() + " started at "
        + startTime_.toString("yyyy-MM-dd HH:mm::ss.zzz"));
    setCaller("ini");
}

Logger::~Logger()
{
    setCaller("log");
    log("# " + qApp->applicationName() + " session ended");
    log("# duration: " + QString::number(startTime_.msecsTo(QDateTime::currentDateTime())) + "ms");
    log("# computing time: " + QString::number(computingTime_) + "ms");
    delete log_.device();
    gLogger = nullptr;
}

//! Writes line to log file, decorated with information on context and timing.
void Logger::log(const QString& lineArg) const
{
    QString line = lineArg;
    static auto lastTime = startTime_;
    const auto currTime = QDateTime::currentDateTime();
    int tDiff = lastTime.msecsTo(currTime);
    lastTime = currTime;
    QString prefix = "[";
    if (caller_=="gui" && line[0]!='#') {
        prefix += "       "; // direct user action: we don't care how long the user was idle
    } else {
        prefix += QString::number(tDiff).rightJustified(5) + "ms";
        computingTime_ += tDiff;
    }
    prefix += " " + level_ + " " + caller_ + "] ";
    log_ << prefix << line << "\n";
    log_.flush();
    if (line.indexOf("##")!=0) { // copy to terminal unless already done by messageHandler
        qterr << line << "\n";
        qterr.flush();
    }
}
