/*
     OpenDCP: Builds Digital Cinema Packages
     Copyright (c) 2010-2011 Terrence Meiczinger, All Rights Reserved

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

#include <QDir>
#include "j2kconversion_dialog.h"

enum J2K_STATE {
    IDLE  = 0,
    START,
    STOP, 
    RUN
};

J2kConversionDialog::J2kConversionDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint);

    connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(stop()));
}

void J2kConversionDialog::init(int imageCount, int threadCount)
{
    QString labelText;

    currentCount = 0;
    cancelled    = 0;
    totalCount   = imageCount;

    progressBar->reset();
    progressBar->setRange(0, totalCount);

    labelText.sprintf("Conversion using %d threads(s)",threadCount);
    labelThreadCount->setText(labelText);

    setButtons(RUN);
}

void J2kConversionDialog::step()
{
    QString labelText;
  
    if (cancelled) {
        return;
    }

    labelText.sprintf("Completed %d of %d.",currentCount,totalCount);

    labelTotal->setText(labelText);
    progressBar->setValue(currentCount);

    // make sure current doesn't exceed total (shouldn't happen)
    if (currentCount < totalCount) {
        currentCount++;
    }
}

void J2kConversionDialog::finished()
{
    QString t;
    QString labelText;

    if (cancelled) {
        t = "cancelled.";
    } else {
        t = "completed.";
        currentCount = totalCount;
     }

    progressBar->setValue(currentCount);
    labelText.sprintf("Completed %d of %d. Conversion ",currentCount,totalCount);
    labelText.append(t);
    labelTotal->setText(labelText);
    setButtons(IDLE);
}

void J2kConversionDialog::stop()
{
    setButtons(STOP);
    labelTotal->setText("Cancelling...");
    cancelled = 1;
    emit cancel();
}

void J2kConversionDialog::setButtons(int state)
{
    switch (state)
    {
        case IDLE:
            buttonClose->setEnabled(true);
            buttonStop->setEnabled(false);
            break;
        case RUN:
            buttonClose->setEnabled(false);
            buttonStop->setEnabled(true);
            break;
        case STOP:
            buttonClose->setEnabled(false);
            buttonStop->setEnabled(false);
            break;
    }
}
