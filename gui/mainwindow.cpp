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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "generatetitle.h"
#include "dialogj2kconversion.h"
#include "dialogmxfconversion.h"
#include "mxf-writer.h"
#include "opendcp.h"
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textEdit = new QPlainTextEdit;

    generateTitle   = new GenerateTitle(this);
    dJ2kConversion  = new DialogJ2kConversion();
    dMxfConversion  = new DialogMxfConversion();
    mxfWriterThread = new MxfWriter(this);

    /* create menus */
    createActions();
    createMenus();

    connectSlots();
    setInitialUiState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSlots()
{
    j2kConnectSlots();
    mxfConnectSlots();
    connectXmlSlots();

    // set initial directory
    lastDir = QString::null;

    // connect tool button to line edits
    connect(&signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(getPath(QWidget*)));
}

// Called every time, when a menu entry of the language menu is called
void MainWindow::slotLanguageChanged(QAction* action)
{
    if (action != 0) {
        // load the language dependant on the action content
        loadLanguage(action->data().toString());
        setWindowIcon(action->icon());
    }
}

void switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);

    // load the new translator
    if (translator.load(filename)) {
        qApp->installTranslator(&translator);
    }
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
    if(m_currLang != rLanguage) {
        m_currLang = rLanguage;
        QLocale::setDefault(QLocale(m_currLang));
        switchTranslator(m_translator, QString(m_langPath + "/opendcp_%1.qm").arg(m_currLang));
        switchTranslator(m_translatorQt, QString("qt_%1.qm").arg(m_currLang));
    }
}

// we create the menu entries dynamically, dependant on the existing translations.
void MainWindow::createLanguageMenu(void)
{
    QActionGroup* langGroup = new QActionGroup(this);
    langGroup->setExclusive(true);

    connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));

    // format systems language
    QString defaultLocale = QLocale::system().name();       // e.g. "en_US"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "en"

#ifdef Q_WS_MAC
    m_langPath = qApp->applicationDirPath();
    m_langPath.truncate(m_langPath.lastIndexOf('/'));
    m_langPath.append("/Resources");
#endif

#ifdef Q_WS_WIN
    m_langPath = QApplication::applicationDirPath().;
    m_langPath.truncate(m_langPath.lastIndexOf('/'));
#endif

#ifdef Q_WS_X11
    m_langPath = "/usr/share/opendcp";
#endif

    m_langPath.append("/translation");

    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("opendcp_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        // get locale extracted by filename
        QString locale;
        locale = fileNames[i];                      // "opendcp_xx.qm"
        locale.truncate(locale.lastIndexOf('.'));   // "opendcp_xx"
        locale.remove(0, locale.indexOf('_') + 1);  // "xx"

        QString lang = QLocale::languageToString(QLocale(locale).language());
        //QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        //QAction *action = new QAction(ico, lang, this);
        QAction *action = new QAction(lang, this);
        action->setCheckable(true);
        action->setData(locale);

        languageMenu->addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale) {
            action->setChecked(true);
        }
    }
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(preferencesAct);

    menuBar()->addSeparator();

    languageMenu = menuBar()->addMenu(tr("&Language"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    createLanguageMenu();
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));


    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    preferencesAct = new QAction(tr("&Preferences"), this);
    preferencesAct->setStatusTip(tr("Application preferences"));
    preferencesAct->setMenuRole(QAction::PreferencesRole);
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(preferences()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
}

void MainWindow::setInitialUiState()
{
    ui->mxfSoundRadio2->setChecked(1);

    // set initial screen indexes
    j2kSetStereoscopicState();
    mxfSetStereoscopicState();
    mxfSetHVState();
    mxfSetSoundState();
    mxfSetSlideState();

    // Check For Kakadu
    QProcess *kdu;
    kdu = new QProcess(this);
    int exitCode = kdu->execute("kdu_compress", QStringList() << "-version");
    if (exitCode) {
        int value = ui->encoderComboBox->findText("Kakadu");
        ui->encoderComboBox->removeItem(value);
    }
    delete kdu;

    // Set thread count
#ifdef Q_WS_WIN
    ui->threadsSpinBox->setMaximum(6);
#endif
    ui->threadsSpinBox->setMaximum(QThreadPool::globalInstance()->maxThreadCount());
    ui->threadsSpinBox->setValue(QThread::idealThreadCount());

    ui->mxfSourceTypeComboBox->setCurrentIndex(0);
    ui->mxfInputStack->setCurrentIndex(0);
    ui->mxfTypeComboBox->setCurrentIndex(1);
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::getPath(QWidget *w)
{
    QString path;
    QString filter;

    if (w->objectName().contains("picture") ||
        w->objectName().contains("Image")   ||
        w->objectName().contains("J2k")) {
        if (w->objectName().contains("picture") && ui->mxfSourceTypeComboBox->currentIndex() == 1) {
            filter = "*.m2v";
            path = QFileDialog::getOpenFileName(this, tr("Choose an mpeg2 file"),lastDir);
        } else {
            path = QFileDialog::getExistingDirectory(this, tr("Choose a directory of images"),lastDir);
        }
    } else if (w->objectName().contains("subIn")) {
        filter = "*.xml";
        path = QFileDialog::getOpenFileName(this, tr("DCDM Subtitle File"),lastDir);
    } else if (w->objectName().contains("MxfOut")) {
        filter = "*.mxf";
        path = QFileDialog::getSaveFileName(this, tr("Save MXF as"),lastDir,filter);
    } else {
        filter = "*.wav";
        path = QFileDialog::getOpenFileName(this, tr("Choose a file to open"),lastDir,filter);
    }

    w->setProperty("text", path);
    lastDir = path;
}

int findSeqOffset(const char str1[], const char str2[])
{
    int i = 0;
    while(1) {
        if(str1[i] != str2[i])
                return i;
        if(str1[i] == '\0' || str2[i] == '\0')
                return 0;
        i++;
    }
}

int MainWindow::checkSequential(const char str1[], const char str2[])
{
    long x,y;
    int  i;
    int  offset = 0;

    if (strlen(str1) != strlen(str2)) {
        return STRING_LENGTH_NOTEQUAL;
    }

    for (i = 0; i < (int) strlen(str1); i++) {
        if (str1[i] != str2[i]) {
            offset = i;
            break;
        }
    }
 
    x = strtol(str1+offset,NULL,10);
    y = strtol(str2+offset,NULL,10);

    if ((y - x) == 1) {
        return DCP_SUCCESS;
    } else {
        return STRING_NOTSEQUENTIAL;
    }
}

/* check if filelist is sequential */
int MainWindow::checkFileSequence(QStringList list)
{
    QString msg;
    int     sequential = DCP_SUCCESS;
    int     x = 0;

    if (list.size() <= 1) {
        return DCP_SUCCESS;
    }

    for (x = 0; (x < (list.size()-1)) && (sequential == DCP_SUCCESS); x++) {
        QFileInfo a(list.at(x));
        QFileInfo b(list.at(x+1));
        sequential = checkSequential((char *)a.completeBaseName().toStdString().c_str(),
                                     (char *)b.completeBaseName().toStdString().c_str());
    }

    if (sequential == STRING_LENGTH_NOTEQUAL) {
        QMessageBox::critical(this, tr("File Sequence Check Error"), tr("All filenames must be of the same length."));
        return DCP_ERROR;
    }

    if (sequential == STRING_NOTSEQUENTIAL) {
        QTextStream(&msg) << "File list does not appear to be sequential between " << list.at(x-1);
        QTextStream(&msg) << " and " << list.at(x) << ". Do you wish to continue?";
        if (QMessageBox::question(this, tr("File Sequence Mismatch"), msg, QMessageBox::No,QMessageBox::Yes) == QMessageBox::No) {
            return DCP_ERROR;
        } else {
            return DCP_NO_ERROR;
        }
    }

    if (sequential == DCP_SUCCESS) {
        return DCP_SUCCESS;
    } else {
        return DCP_ERROR;
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event != 0) {
        switch(event->type()) {
        // this event is send if a translator is loaded
        case QEvent::LanguageChange:
            //ui.retranslateUi(this);
            break;
        // this event is send, if the system, language changes
        case QEvent::LocaleChange:
            {
                QString locale = QLocale::system().name();
                locale.truncate(locale.lastIndexOf('_'));
                loadLanguage(locale);
            }
            break;
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::about()
{
    QString msg;
    QTextStream(&msg) << OPENDCP_NAME << " Version " << OPENDCP_VERSION << "\n\n";
    QTextStream(&msg) << OPENDCP_COPYRIGHT << "\n\n" << OPENDCP_LICENSE << "\n\n" << OPENDCP_WEBSITE << "\n\n";
    QMessageBox::about(this, "About OpenDCP",msg);
}

void MainWindow::preferences()
{
    QString msg = "goo";
}
