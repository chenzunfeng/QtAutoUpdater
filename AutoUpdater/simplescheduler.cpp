#include "simplescheduler.h"
#include <QTimerEvent>
#include "updater.h"

SimpleScheduler::SimpleScheduler(QObject *parent) :
	QObject(parent),
	timerHash()
{}

int SimpleScheduler::startSchedule(int msecs, bool repeated, const QVariant &parameter)
{
	if(msecs < 0) {
		qCWarning(logQtAutoUpdater, "Cannot schedule update tasks for the past!");
		return 0;
	}

	const auto id = this->startTimer(msecs, Qt::VeryCoarseTimer);
	if(id != 0)
		this->timerHash.insert(id, {repeated, parameter});
	return id;
}

int SimpleScheduler::startSchedule(const QDateTime &when, const QVariant &parameter)
{
	const auto delta = QDateTime::currentDateTime().msecsTo(when);
	if(delta > (qint64)INT_MAX) {
		qCWarning(logQtAutoUpdater, "Time interval to big, timepoint to far in the future.");
		return 0;
	} else
		return this->startSchedule((int)delta, false, parameter);
}

void SimpleScheduler::cancelSchedule(int id)
{
	this->killTimer(id);
	this->timerHash.remove(id);
}

void SimpleScheduler::timerEvent(QTimerEvent *event)
{
	const auto id = event->timerId();
	const auto info = this->timerHash.value(id, {false, QVariant()});
	if(!info.first)
		this->cancelSchedule(id);
	emit scheduleTriggered(info.second);
	event->accept();
}
