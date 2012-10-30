/*
     : Builds Digital Cinema Packages
     Copyright (c) 2010-2012 Terrence Meiczinger, All Rights Reserved

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGui>
#include <QDir>
#include <stdio.h>
#include <stdlib.h>
#include <opendcp.h>

int foo(void *p)
{
    QProgressDialog *progress = static_cast<QProgressDialog *>(p);
    progress->setValue(progress->value()+1);

    if (progress->wasCanceled()) {
        progress->hide();
        return 1;
    }

    return 0;
}

QString MainWindow::calculateDigest(opendcp_t *opendcp, QString text, QString filename)
{
    int pg = 0;
    char assetDigest[40];

    QFileInfo file(filename);
    qint64 len = file.size() / FILE_READ_SIZE;

    QString msg;
    msg.sprintf("Calculating %s %s SHA1 Digest", text.toUtf8().data(), file.fileName().toUtf8().data());

    QProgressDialog *progress = new QProgressDialog(msg, "Cancel", 0, len+1, this);
    progress->setWindowModality(Qt::WindowModal);

    opendcp->dcp.sha1_update.callback = foo;
    opendcp->dcp.sha1_update.argument = progress;

    if (calculate_digest(opendcp, filename.toUtf8().data(), assetDigest) == OPENDCP_CALC_DIGEST) {
        return NULL;
    }

    progress->hide();
    //qApp->processEvents();

    /*
    while (!src.atEnd()) {
        QByteArray bytearray = src.read(4096);
        dst.write(bytearray);
        qint64 act = bytearray.length();
        pg = pg + act;
        progress->setValue(pg);
        if (progress->wasCanceled()) {
            progress->hide();
            break;
        }

    }
    */

   return assetDigest;
}
