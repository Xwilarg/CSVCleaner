#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "MainWindow.hpp"

namespace CSVCleaner
{
    MainWindow::MainWindow() noexcept
        : QMainWindow(nullptr), _mainWidget(this),
          _fileMenu(menuBar()->addMenu(tr("File"))), _helpMenu(menuBar()->addMenu(tr("?"))),
          _previewBox(tr("Preview"), &_mainWidget), _configBox(tr("Configuration"), &_mainWidget),
          _defaultSeparatorBox(tr("Default separator"), &_configBox), _defaultNewLineBox(tr("Default newline"), &_previewBox),
          _previewTab(&_previewBox),
          _previewLayout(&_previewBox), _mainLayout(&_mainWidget),
          _configLayout(&_configBox), _defaultSeparatorLayout(&_defaultSeparatorBox), _defaultNewLineLayout(&_defaultNewLineBox),
          _defaultSeparatorEdit(&_defaultSeparatorBox), _defaultNewLineEdit(&_defaultNewLineBox),
          _openAction(std::make_unique<QAction>(tr("Open"), this)), _quitAction(std::make_unique<QAction>(tr("Quit"), this)),
          _aboutQtAction(std::make_unique<QAction>(tr("About Qt"), this)),
          _csvText(&_previewTab), _csvTable(&_previewTab),
          _documentContent("")
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
        _mainLayout.addWidget(&_configBox);
        _previewLayout.addWidget(&_previewTab);
        _configLayout.addWidget(&_defaultSeparatorBox);
        _configLayout.addWidget(&_defaultNewLineBox);
        _defaultSeparatorLayout.addWidget(&_defaultSeparatorEdit);
        _defaultNewLineLayout.addWidget(&_defaultNewLineEdit);
        _previewTab.addTab(&_csvText, tr("Raw"));
        _previewTab.addTab(&_csvTable, tr("Table"));
        _csvText.setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
        _defaultSeparatorEdit.setText(";");
        _defaultNewLineEdit.setText("\\n");

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

    std::string MainWindow::EscapeString(std::string &&str) const noexcept
    {
        std::string finalStr = "";
        std::string::const_iterator it = str.begin();
        while (it != str.end())
        {
            char c = *it++;
            if (c == '\\' && it != str.end())
            {
                switch (*it++)
                {
                case 'n': c = '\n'; break;
                case 't': c = '\t'; break;
                case 'r': c = '\r'; break;
                }
            }
            finalStr += c;
        }
        return (finalStr);
    }

    void MainWindow::SetDataTable() noexcept
    {
        _csvTable.reset();
        std::string separator = EscapeString(_defaultSeparatorEdit.text().toStdString());
        std::string newLine = EscapeString(_defaultNewLineEdit.text().toStdString());
        QList<QString> header;
        std::string content = _documentContent.toStdString();
        std::string firstLine = content.substr(0, content.find("\n"));
        int i = 0;
        size_t pos = 0;
        std::string token;
        while ((pos = firstLine.find(separator)) != std::string::npos) {
            token = firstLine.substr(0, pos);
            header.push_back(QString::fromStdString(token));
            firstLine.erase(0, pos + separator.size());
            i++;
        }
        std::vector<std::string> allLines;
        while ((pos = content.find(newLine)) != std::string::npos) {
            token = content.substr(0, pos);
            allLines.push_back(token);
            content.erase(0, pos + newLine.size());
        }
        if (allLines.size() == 1)
            return;
        _csvTable.setRowCount(allLines.size() - 1);
        _csvTable.setColumnCount(header.size());
        _csvTable.setHorizontalHeaderLabels(header);
        for (size_t y = 1; y < allLines.size(); y++)
        {
            i = 0;
            while ((pos = allLines[y].find(separator)) != std::string::npos) {
                token = allLines[y].substr(0, pos);
                _csvTable.setItem(y, i, new QTableWidgetItem(QString::fromStdString(token)));
                allLines[y].erase(0, pos + separator.size());
                i++;
            }
        }
    }
}
