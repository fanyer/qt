/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef PIXMAPNODE_H
#define PIXMAPNODE_H

#include "node.h"
#include "texturematerial.h"

class PixmapNode : public GeometryNode
{
public:
    PixmapNode(const QRectF &targetRect, const QPixmap &pixmap, const QRectF &sourceRect = QRectF());

    void setRect(const QRectF &rect);
    QRectF rect() const { return m_target_rect; }

    void setSourceRect(const QRectF &rect);
    QRectF sourceRect() const { return m_source_rect; }

    void setRects(const QRectF &targetRect, const QRectF &sourceRect);

    virtual NodeSubType subType() const { return PixmapNodeSubType; }

private:
    void updateGeometry();

    QRectF m_target_rect;
    QPixmap m_pixmap;
    QRectF m_source_rect;
    TextureMaterial m_material;
};


class PixmapObjectNode : public QObject, public PixmapNode
{
    Q_OBJECT
    Q_PROPERTY(QPointF centerPos READ centerPos WRITE setCenterPos)

public:
    PixmapObjectNode(const QRectF &targetRect, const QPixmap &pixmap, const QRectF &sourceRect = QRectF())
        : PixmapNode(targetRect, pixmap, sourceRect)
    {
    }

    void setCenterPos(const QPointF &pos) {
        QRectF r = rect();
        setRect(QRectF(pos.x() - r.width() / 2, pos.y() - r.height() / 2, r.width(), r.height()));
    }

    QPointF centerPos() const {
        return rect().center();
    }
};


#endif // PIXMAPNODE_H