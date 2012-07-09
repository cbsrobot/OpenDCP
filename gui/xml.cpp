/*
     : Builds Digital Cinema Packages
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "generatetitle.h"
#include <QtGui>
#include <QDir>
#include <stdio.h>
#include <stdlib.h>
#include <opendcp.h>

asset_t pictureAsset;
asset_t soundAsset;
asset_t subtitleAsset;

enum ASSET_INDEX {
    PICTURE = 0,
    SOUND,
    SUBTITLE
};

void MainWindow::connectXmlSlots()
{
    // connect slots
    connect(ui->reelPictureButton,         SIGNAL(clicked()),         this, SLOT(setPictureTrack()));
    connect(ui->reelSoundButton,           SIGNAL(clicked()),         this, SLOT(setSoundTrack()));
    connect(ui->reelSubtitleButton,        SIGNAL(clicked()),         this, SLOT(setSubtitleTrack()));
    connect(ui->reelPictureOffsetSpinBox,  SIGNAL(valueChanged(int)), this, SLOT(updatePictureDuration()));
    connect(ui->reelSubtitleOffsetSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateSubtitleDuration()));
    connect(ui->reelSoundOffsetSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(updateSoundDuration()));
    connect(ui->createDcpButton,           SIGNAL(clicked()),         this, SLOT(startDcp()));
    connect(ui->cplTitleGenButton,         SIGNAL(clicked()),         this, SLOT(getTitle()));
}

void MainWindow::getTitle() {
    if (generateTitle->exec()) {
        QString title = generateTitle->getTitle();
        ui->cplTitleEdit->setText(title);
    }
}

int MainWindow::mxfCopy(QString source, QString destination) {
    QFileInfo   sourceFileInfo;
    QFileInfo   destinationFileInfo;

    if (source == NULL) {
        return OPENDCP_NO_ERROR;
    }


    // set source/destination files
    sourceFileInfo.setFile(source);
    destinationFileInfo.setFile(destination + "/" + sourceFileInfo.fileName());

    // no need to copy if the source/destination are the same
    if (sourceFileInfo.absoluteFilePath() == destinationFileInfo.absoluteFilePath()) {
        return OPENDCP_NO_ERROR;
    }

    // check if file exists
    if (destinationFileInfo.isFile()) {
        if (!ui->overwriteMxfCB->isChecked()) {
            return OPENDCP_NO_ERROR;
        }
        QFile::remove(destinationFileInfo.absoluteFilePath());
    }

    // copy/move the file
    if (ui->rbMoveMxf->isChecked()) {
        QFile::rename(sourceFileInfo.absoluteFilePath(), destinationFileInfo.absoluteFilePath());
    } else {
        fileCopy(sourceFileInfo.absoluteFilePath(), destinationFileInfo.absoluteFilePath());
    }

    return OPENDCP_NO_ERROR;
}

void MainWindow::startDcp()
{
    QString     path;
    QMessageBox msgBox;
    QString     DCP_FAIL_MSG;

    asset_list_t reelList[MAX_REELS];

    // get dcp destination directory
    path = QFileDialog::getExistingDirectory(this, tr("Choose destination folder"), lastDir);

    if (path.isEmpty()) {
        return;
    }

    lastDir = path;

    opendcp_t *xmlContext = opendcp_create();

    DCP_FAIL_MSG = tr("DCP Creation Failed");

    // process options
    xmlContext->log_level = 0;

    if (ui->digitalSignatureCheckBox->checkState()) {
        xmlContext->xml_signature.sign = 1;
        xmlContext->xml_signature.use_external = 0;
    }

    dcp_log_init(xmlContext->log_level, ".log");

    // set XML attribues
    strcpy(xmlContext->xml.title, ui->cplTitleEdit->text().toUtf8().constData());
    strcpy(xmlContext->xml.annotation, ui->cplAnnotationEdit->text().toUtf8().constData());
    strcpy(xmlContext->xml.issuer, ui->cplIssuerEdit->text().toUtf8().constData());
    strcpy(xmlContext->xml.kind, ui->cplKindComboBox->currentText().toUtf8().constData());
    strcpy(xmlContext->xml.rating, ui->cplRatingComboBox->currentText().toUtf8().constData());

    // check picture track is supplied
    if (ui->reelPictureEdit->text().isEmpty()) {
        QMessageBox::critical(this, DCP_FAIL_MSG,
                             tr("An MXF picture track is required."));
        goto Done;
    }

    // check durations
    if ((!ui->reelSoundEdit->text().isEmpty() && ui->reelPictureDurationSpinBox->value() != ui->reelSoundDurationSpinBox->value()) ||
        (!ui->reelSubtitleEdit->text().isEmpty() && ui->reelPictureDurationSpinBox->value() != ui->reelSubtitleDurationSpinBox->value())) {
        QMessageBox::critical(this, DCP_FAIL_MSG,
                              tr("The duration of all MXF tracks must be the same."));
        goto Done;
    }

    // set asset filenames
    reelList->asset_count = 0;
    if (!ui->reelPictureEdit->text().isEmpty()) {
        strcpy(reelList[0].asset_list[PICTURE].filename, ui->reelPictureEdit->text().toUtf8().constData());
        reelList->asset_count++;
    }
    if (!ui->reelSoundEdit->text().isEmpty()) {
        strcpy(reelList[0].asset_list[SOUND].filename, ui->reelSoundEdit->text().toUtf8().constData());
        reelList->asset_count++;
    }
    if (!ui->reelSubtitleEdit->text().isEmpty()) {
        strcpy(reelList[0].asset_list[SUBTITLE].filename, ui->reelSubtitleEdit->text().toUtf8().constData());
        reelList->asset_count++;
    }

    // add pkl to the DCP (only one PKL currently support)
    add_pkl(xmlContext);

    // add cpl to the DCP/PKL (only one CPL currently support)
    add_cpl(xmlContext, &xmlContext->pkl[0]);

    if (add_reel(xmlContext, &xmlContext->pkl[0].cpl[0],reelList[0]) != OPENDCP_NO_ERROR) {
        QMessageBox::critical(this, DCP_FAIL_MSG,
                              tr("Could not add reel to CPL."));
        goto Done;
    }

    // adjust durations
    xmlContext->pkl[0].cpl[0].reel[0].asset[PICTURE].duration     = ui->reelPictureDurationSpinBox->value();
    xmlContext->pkl[0].cpl[0].reel[0].asset[PICTURE].entry_point  = ui->reelPictureOffsetSpinBox->value();
    xmlContext->pkl[0].cpl[0].reel[0].asset[SOUND].duration       = ui->reelSoundDurationSpinBox->value();
    xmlContext->pkl[0].cpl[0].reel[0].asset[SOUND].entry_point    = ui->reelSoundOffsetSpinBox->value();
    xmlContext->pkl[0].cpl[0].reel[0].asset[SUBTITLE].duration    = ui->reelSubtitleDurationSpinBox->value();
    xmlContext->pkl[0].cpl[0].reel[0].asset[SUBTITLE].entry_point = ui->reelSubtitleOffsetSpinBox->value();

    if (validate_reel(xmlContext,&xmlContext->pkl[0].cpl[0],0) != OPENDCP_NO_ERROR) {
        QMessageBox::critical(this, DCP_FAIL_MSG, tr("Could not valiate reel."));
        goto Done;
    }

    sprintf(xmlContext->pkl[0].cpl[0].filename,"%s/%s_cpl.xml", path.toUtf8().constData(), xmlContext->pkl[0].cpl[0].uuid);
    sprintf(xmlContext->pkl[0].filename,"%s/%s_pkl.xml", path.toUtf8().constData(), xmlContext->pkl[0].uuid);

    if (xmlContext->ns == XML_NS_SMPTE) {
        sprintf(xmlContext->assetmap.filename,"%s/ASSETMAP.xml",path.toUtf8().constData());
        sprintf(xmlContext->volindex.filename,"%s/VOLINDEX.xml",path.toUtf8().constData());
    } else {
        sprintf(xmlContext->assetmap.filename,"%s/ASSETMAP",path.toUtf8().constData());
        sprintf(xmlContext->volindex.filename,"%s/VOLINDEX",path.toUtf8().constData());
    }

    // write XML Files
    if (write_cpl(xmlContext,&xmlContext->pkl[0].cpl[0]) != OPENDCP_NO_ERROR) {
        QMessageBox::critical(this, DCP_FAIL_MSG, tr("Failed to create composition playlist."));
        goto Done;
    }
    if (write_pkl(xmlContext,&xmlContext->pkl[0]) != OPENDCP_NO_ERROR) {
        QMessageBox::critical(this, DCP_FAIL_MSG, tr("Failed to create packaging list."));
        goto Done;
    }
    if (write_volumeindex(xmlContext) != OPENDCP_NO_ERROR) {
        QMessageBox::critical(this, DCP_FAIL_MSG, tr("Failed to create volume index."));
        goto Done;
    }
    if (write_assetmap(xmlContext) != OPENDCP_NO_ERROR) {
        QMessageBox::critical(this, DCP_FAIL_MSG, tr("Failed to create assetmap."));
        goto Done;
    }

    // copy picture mxf
    mxfCopy(QString::fromUtf8(xmlContext->pkl[0].cpl[0].reel[0].asset[PICTURE].filename), path);

    // copy audio mxf
    mxfCopy(QString::fromUtf8(xmlContext->pkl[0].cpl[0].reel[0].asset[SOUND].filename), path);

    // copy subtitle mxf
    mxfCopy(QString::fromUtf8(xmlContext->pkl[0].cpl[0].reel[0].asset[SUBTITLE].filename), path);

    msgBox.setText("DCP Created successfully");
    msgBox.exec();


Done:
    opendcp_delete(xmlContext);

    return;
}

void MainWindow::updatePictureDuration()
{
    int offset = ui->reelPictureOffsetSpinBox->value();
    ui->reelPictureDurationSpinBox->setMaximum(pictureAsset.intrinsic_duration-offset);
}

void MainWindow::updateSoundDuration()
{
    int offset = ui->reelSoundOffsetSpinBox->value();
    ui->reelSoundDurationSpinBox->setMaximum(soundAsset.intrinsic_duration-offset);
}

void MainWindow::updateSubtitleDuration()
{
    int offset = ui->reelSubtitleOffsetSpinBox->value();
    ui->reelSubtitleDurationSpinBox->setMaximum(subtitleAsset.intrinsic_duration-offset);
}

void MainWindow::setPictureTrack()
{
    QString path;
    QString filter = "*.mxf";

    path = QFileDialog::getOpenFileName(this, tr("Choose a file to open"), lastDir, filter);

    if (path.isEmpty()) {
        return;
    }

    QFileInfo fi(path);
    lastDir = fi.absolutePath();

    if (get_file_essence_class(path.toUtf8().data()) != ACT_PICTURE) {
        QMessageBox::critical(this, tr("Not a Picture Track"),
                              tr("The selected file is not a valid MXF picture track."));
    } else {
        ui->reelPictureEdit->setProperty("text", path);
        strcpy(pictureAsset.filename, ui->reelPictureEdit->text().toUtf8().constData());
        read_asset_info(&pictureAsset);
        ui->reelPictureDurationSpinBox->setMaximum(pictureAsset.intrinsic_duration);
        ui->reelPictureOffsetSpinBox->setMaximum(pictureAsset.intrinsic_duration-1);
        ui->reelPictureDurationSpinBox->setValue(pictureAsset.duration);
    }

    return;
}

void MainWindow::setSoundTrack()
{
    QString path;
    QString filter = "*.mxf";

    path = QFileDialog::getOpenFileName(this, tr("Choose a file to open"), lastDir, filter);

    if (path.isEmpty()) {
        return;
    }

    QFileInfo fi(path);
    lastDir = fi.absolutePath();

    if (get_file_essence_class(path.toUtf8().data()) != ACT_SOUND) {
        QMessageBox::critical(this, tr("Not a Sound Track"),
                             tr("The selected file is not a valid MXF sound track."));
    } else {
        ui->reelSoundEdit->setProperty("text", path);
        strcpy(soundAsset.filename, ui->reelSoundEdit->text().toUtf8().constData());
        read_asset_info(&soundAsset);
        ui->reelSoundDurationSpinBox->setValue(soundAsset.duration);
        ui->reelSoundDurationSpinBox->setMaximum(soundAsset.intrinsic_duration);
        ui->reelSoundOffsetSpinBox->setMaximum(soundAsset.intrinsic_duration-1);
    }

    return;
}

void MainWindow::setSubtitleTrack()
{
    QString path;
    QString filter = "*.mxf";

    path = QFileDialog::getOpenFileName(this, tr("Choose an file to open"), lastDir, filter);

    if (path.isEmpty()) {
        return;
    }

    QFileInfo fi(path);
    lastDir = fi.absolutePath();

    if (get_file_essence_class(path.toUtf8().data()) != ACT_TIMED_TEXT) {
        QMessageBox::critical(this, tr("Not a Subtitle Track"),
                              tr("The selected file is not a valid MXF subtitle track."));
    } else {
        ui->reelSubtitleEdit->setProperty("text", path);
        strcpy(subtitleAsset.filename, ui->reelSubtitleEdit->text().toUtf8().constData());
        read_asset_info(&subtitleAsset);
        ui->reelSubtitleDurationSpinBox->setValue(subtitleAsset.duration);
        ui->reelSubtitleDurationSpinBox->setMaximum(subtitleAsset.intrinsic_duration);
        ui->reelSubtitleOffsetSpinBox->setMaximum(subtitleAsset.intrinsic_duration-1);
    }

    return;
}
