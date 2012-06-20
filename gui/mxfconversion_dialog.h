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

#ifndef MXFCONVERSIONDIALOG_H
#define MXFCONVERSIONDIALOG_H

#include "ui_conversion.h"

class MxfConversionDialog : public QDialog, private Ui::ConversionDialog {
    Q_OBJECT

public:
    MxfConversionDialog(QWidget *parent = 0);
    void init(int nFrames, QString outputFile);

private:
    int totalCount;
    int currentCount;
    int cancelled;
    QString mxfOutputFile;
    void setButtons(int);

signals:
    void cancel();

public slots:
    void finished(int);
    void step();

private slots:
    void stop();
};

#endif // MXFCONVERSIONDIALOG_H
