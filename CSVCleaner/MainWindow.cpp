#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "MainWindow.hpp"

namespace CSVCleaner
{
    MainWindow::MainWindow() noexcept
        : QMainWindow(nullptr), _mainWidget(this),
          _fileMenu(menuBar()->addMenu(tr("File"))), _helpMenu(menuBar()->addMenu(tr("?"))),
          _previewBox(tr("Preview"), &_mainWidget),
          _previewTab(&_previewBox),
          _previewLayout(&_previewBox), _mainLayout(&_mainWidget),
          _openAction(std::make_unique<QAction>(tr("Open"), this)), _quitAction(std::make_unique<QAction>(tr("Quit"), this)),
          _aboutQtAction(std::make_unique<QAction>(tr("About Qt"), this)),
          _csvText(&_previewTab), _csvTable(&_previewTab), _documentContent("")
    {
        constexpr int xSize = 600, ySize = 400;
        setWindowTitle("CSVCleaner");
        resize(xSize, ySize);

        _openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
        _fileMenu->addAction(_openAction.get());
        _fileMenu->addAction(_quitAction.get());
        _helpMenu->addAction(_aboutQtAction.get());

        setCentralWidget(&_mainWidget);
        _mainLayout.addWidget(&_previewBox);
        _previewLayout.addWidget(&_previewTab);
        _previewTab.addTab(&_csvText, tr("Raw"));
        _previewTab.addTab(&_csvTable, tr("Table"));
        _csvText.setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

        connect(_openAction.get(), SIGNAL(triggered()), this, SLOT(OpenFile()));
        connect(_quitAction.get(), SIGNAL(triggered()), this, SLOT(ExitApp()));
        connect(_aboutQtAction.get(), SIGNAL(triggered()), this, SLOT(AboutQt()));
        connect(&_previewTab, SIGNAL(currentChanged(int)), this, SLOT(OnTableLoad(int)));
    }

    void MainWindow::OpenFile()
    {
        QString path(QFileDialog::getOpenFileName(this, tr("Open CSV"), ".", tr("CSV (*.csv);;Other (*)")));
        if (path != "")
        {
            QFile file(path);
            file.open(QIODevice::ReadOnly);
            _documentContent = QString(file.readAll());
            _csvText.document()->setPlainText(_documentContent);
            file.close();
        }
    }

    void MainWindow::ExitApp() const noexcept
    {
        std::exit(0);
    }

    void MainWindow::AboutQt() const noexcept
    {
        QMessageBox::aboutQt(nullptr);
    }

    void MainWindow::OnTableLoad(int index) noexcept
    {
        if (index == 1)
            SetDataTable();
    }

    void MainWindow::SetDataTable() noexcept
    {
        _csvTable.reset();
        QList<QString> header;
        std::string content = _documentContent.toStdString();
        std::string firstLine = content.substr(0, content.find("\n"));
        int i = 0;
        size_t pos = 0;
        std::string token;
        while ((pos = firstLine.find(";")) != std::string::npos) {
            token = firstLine.substr(0, pos);
            header.push_back(QString::fromStdString(token));
            firstLine.erase(0, pos + 1);
            i++;
        }
        std::vector<std::string> allLines;
        while ((pos = content.find("\n")) != std::string::npos) {
            token = content.substr(0, pos);
            allLines.push_back(token);
            content.erase(0, pos + 1);
        }
        if (allLines.size() == 1)
            return;
        _csvTable.setRowCount(allLines.size() - 1);
        _csvTable.setColumnCount(header.size());
        _csvTable.setHorizontalHeaderLabels(header);
        for (size_t y = 1; y < allLines.size(); y++)
        {
            i = 0;
            while ((pos = allLines[y].find(";")) != std::string::npos) {
                token = allLines[y].substr(0, pos);
                _csvTable.setItem(y, i, new QTableWidgetItem(QString::fromStdString(token)));
                allLines[y].erase(0, pos + 1);
                i++;
            }
        }
    }
}
