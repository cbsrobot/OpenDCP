/*
     OpenDCP: Builds Digital Cinema Packages
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

#include <QDir>
#include "mxfconversion_dialog.h"
#include "ui_conversion.h"

MxfConversionDialog::MxfConversionDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint);

    this->setWindowTitle("MXF Creation");

    connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(buttonStop,  SIGNAL(clicked()), this, SLOT(stop()));
}

void MxfConversionDialog::init(int nFrames, QString outputFile)
{
    currentCount  = 0;
    totalCount    = nFrames;

    cancelled     = 0;

    mxfOutputFile = outputFile;

    progressBar->reset();
    progressBar->setMinimum(0);
    progressBar->setMaximum(totalCount);

    setButtons(1);
}

void MxfConversionDialog::step()
{
    QString labelText;

    labelText.sprintf("%s  [Writing %d of %d]",mxfOutputFile.toAscii().constData(),currentCount,totalCount);
    labelTotal->setText(labelText);
    progressBar->setValue(currentCount);

    // make sure current doesn't exceed total (shouldn't happen)
    if (currentCount < totalCount) {
        currentCount++;
    }
}

void MxfConversionDialog::stop()
{
    setButtons(2);
    labelTotal->setText("Cancelling...");
    cancelled = 1;
    emit cancel();
}

void MxfConversionDialog::finished(int result)
{
    QString t;
    QString labelText;

    progressBar->setValue(currentCount);

    if (result) {
        t = "failed";

    } else {
        t = "succeeded";
    }

    if (cancelled) {
        t = "cancelled";
    }

    labelText.sprintf("Wrote %d of %d. MXF file %s creation ",currentCount,totalCount,mxfOutputFile.toAscii().constData());
    labelText.append(t);
    labelTotal->setText(labelText);

    setButtons(0);
}

void MxfConversionDialog::setButtons(int state)
{
    switch (state)
    {
        case 0:
            // stopped
            buttonClose->setEnabled(true);
            buttonStop->setEnabled(false);
            break;
        case 1:
            // running
            buttonClose->setEnabled(false);
            buttonStop->setEnabled(true);
            break;
        case 2:
            // stopping
            buttonClose->setEnabled(false);
            buttonStop->setEnabled(false);
            break;
    }
}
