/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDEVENTTHREAD_H
#define QWAYLANDEVENTTHREAD_H

#include "qwaylanddisplay.h"

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QSocketNotifier>
#include <QtCore/QMutex>

class QWaylandEventThread : public QObject
{
    Q_OBJECT
public:
    explicit QWaylandEventThread(QWaylandDisplay *display);
    ~QWaylandEventThread();

    static int sourceUpdate(uint32_t mask, void *data);

    void waitForScreens();

signals:
public slots:
    void runningInThread();
    void readEvents();
    void flushRequests();
private:
    QWaylandDisplay *mDisplay;
    QSocketNotifier *mReadNotifier;
    QSocketNotifier *mWriteNotifier;
    int mFd;
    QWaitCondition mWaitForScreens;
    bool mScreensInitialized;

    static const struct wl_output_listener outputListener;
    static void displayHandleGlobal(struct wl_display *display,
                                    uint32_t id,
                                    const char *interface,
                                    uint32_t version, void *data);
    static void outputHandleGeometry(void *data,
                                     struct wl_output *output,
                                     int32_t x, int32_t y,
                                     int32_t width, int32_t height);
};

#endif // QWAYLANDEVENTTHREAD_H