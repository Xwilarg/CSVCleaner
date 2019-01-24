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
          _modifBox(tr("Modification"), &_mainWidget),
          _defaultSeparatorBox(tr("Default separator"), &_configBox), _defaultNewLineBox(tr("Default newline"), &_previewBox),
          _previewTab(&_previewBox),
          _previewLayout(&_previewBox), _mainLayout(&_mainWidget), _modifLayout(&_modifBox),
          _configLayout(&_configBox), _defaultSeparatorLayout(&_defaultSeparatorBox), _defaultNewLineLayout(&_defaultNewLineBox),
          _defaultSeparatorEdit(&_defaultSeparatorBox), _defaultNewLineEdit(&_defaultNewLineBox),
          _openAction(std::make_unique<QAction>(tr("Open"), this)), _quitAction(std::make_unique<QAction>(tr("Quit"), this)),
          _aboutQtAction(std::make_unique<QAction>(tr("About Qt"), this)), _saveAction(std::make_unique<QAction>(tr("Save"), this)),
          _saveAsAction(std::make_unique<QAction>(tr("Save as..."), this)),
          _csvText(&_previewTab), _csvTable(&_previewTab),
          _columnSelection(&_modifBox), _selectedLineLabel(tr("Selected columns:\n"), &_modifBox),
          _availableLineList(), _selectedLineList(),
          _selectedAdd(tr("Add"), &_modifBox), _selectedReset(tr("Reset"), &_modifBox),
          _selectedExport(tr("Export")),
          _saveStr("")
    {
        constexpr int xSize = 600, ySize = 600;
        setWindowTitle("CSVCleaner");
        resize(xSize, ySize);

        _openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
        _saveAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
        _saveAsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
        _fileMenu->addAction(_saveAction.get());
        _fileMenu->addAction(_saveAsAction.get());
        _fileMenu->addAction(_openAction.get());
        _fileMenu->addAction(_quitAction.get());
        _helpMenu->addAction(_aboutQtAction.get());

        setCentralWidget(&_mainWidget);
        _mainLayout.addWidget(&_previewBox);
        _mainLayout.addWidget(&_configBox);
        _mainLayout.addWidget(&_modifBox);
        _previewLayout.addWidget(&_previewTab);
        _configLayout.addWidget(&_defaultSeparatorBox);
        _configLayout.addWidget(&_defaultNewLineBox);
        _defaultSeparatorLayout.addWidget(&_defaultSeparatorEdit);
        _defaultNewLineLayout.addWidget(&_defaultNewLineEdit);
        _modifLayout.addWidget(&_columnSelection, 0, 0, 1, 0);
        _modifLayout.addWidget(&_selectedAdd);
        _modifLayout.addWidget(&_selectedReset, 1, 1);
        _modifLayout.addWidget(&_selectedLineLabel);
        _modifLayout.addWidget(&_selectedExport, 3, 0, 1, 0);

        _previewTab.addTab(&_csvText, tr("Raw"));
        _previewTab.addTab(&_csvTable, tr("Table"));
        _csvText.setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
        _defaultSeparatorEdit.setText(";");
        _defaultNewLineEdit.setText("\\n");

        connect(_saveAction.get(), SIGNAL(triggered()), this, SLOT(SaveFile()));
        connect(_saveAsAction.get(), SIGNAL(triggered()), this, SLOT(SaveFileAs()));
        connect(_openAction.get(), SIGNAL(triggered()), this, SLOT(OpenFile()));
        connect(_quitAction.get(), SIGNAL(triggered()), this, SLOT(ExitApp()));
        connect(_aboutQtAction.get(), SIGNAL(triggered()), this, SLOT(AboutQt()));
        connect(&_previewTab, SIGNAL(currentChanged(int)), this, SLOT(OnTableLoad(int)));
        connect(&_selectedAdd, SIGNAL(clicked()), this, SLOT(AddElement()));
        connect(&_selectedReset, SIGNAL(clicked()), this, SLOT(ResetElements()));
        connect(&_selectedExport, SIGNAL(clicked()), this, SLOT(ExportElements()));
    }

    void MainWindow::SaveFile() const noexcept
    {
        SaveFileInternal();
    }

    void MainWindow::SaveFileAs() noexcept
    {
        QString path(QFileDialog::getSaveFileName(this, tr("Save As"), "", tr("CSV (*.csv);;All Files (*)")));
        if (path != "")
        {
            _saveStr = path;
            SaveFileInternal();
        }
    }

    void MainWindow::OpenFile() noexcept
    {
        QString path(QFileDialog::getOpenFileName(this, tr("Open CSV"), "", tr("CSV (*.csv);;All Files (*)")));
        if (path != "")
        {
            _saveStr = path;
            QFile file(path);
            file.open(QIODevice::ReadOnly);
            _csvText.document()->setPlainText(QString(file.readAll()));
            file.close();
            LoadDataInfo();
            ResetElements();
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

    void MainWindow::AddElement() noexcept
    {
        const QString &element = _columnSelection.currentText();
        if (element  != "")
        {
            _selectedLineLabel.setText(_selectedLineLabel.text() + element + '\n');
            _selectedLineList.push_back(element);
            _availableLineList.removeOne(element);
            UpdateColumnList();
        }
    }

    void MainWindow::ResetElements() noexcept
    {
        _selectedLineLabel.setText(tr("Selected columns:\n"));
        _selectedLineList.clear();
        LoadDataInfo();
    }

    void MainWindow::ExportElements() noexcept
    {
        if (_selectedLineList.size() == 0)
            QMessageBox::warning(this, tr("Invalid operation"), tr("There is nothing to export"));
        else
        {
            QString path(QFileDialog::getSaveFileName(this, tr("Save As"), "", tr("CSV (*.csv);;All Files (*)")));
            if (path != "")
            {
                QFile file(path);
                file.open(QIODevice::WriteOnly);
                std::vector<std::string> allLines = GetAllLines();
                const std::string &separator = EscapeString(_defaultSeparatorEdit.text().toStdString());
                const std::string &newLine = EscapeString(_defaultNewLineEdit.text().toStdString());
                size_t pos;
                int i = 0;
                std::string token;
                QList<int> keepHeaders;
                std::string &firstLine = allLines[0];
                size_t size;
                while ((pos = FindSeparatorOrNewLine(firstLine, separator, newLine, size)) != std::string::npos) {
                    token = firstLine.substr(0, pos);
                    if (_selectedLineList.contains(QString::fromStdString(token)))
                        keepHeaders.push_back(i);
                    firstLine.erase(0, pos + size);
                    i++;
                }
                std::string finalCsv = "";
                bool isFirst = true;
                for (const QString &str : _selectedLineList)
                {
                    if (!isFirst)
                        finalCsv += separator;
                    isFirst = false;
                    finalCsv += str.toStdString();
                }
                finalCsv += newLine;
                for (size_t y = 1; y < allLines.size(); y++)
                {
                    i = 0;
                    isFirst = true;
                    while ((pos = FindSeparatorOrNewLine(allLines[y], separator, newLine, size)) != std::string::npos) {
                        token = allLines[y].substr(0, pos);
                        if (keepHeaders.contains(i))
                        {
                            if (!isFirst)
                                finalCsv += separator;
                            finalCsv += token;
                            isFirst = false;
                        }
                        allLines[y].erase(0, pos + size);
                        i++;
                    }
                    finalCsv += newLine;
                }
                file.write(QString::fromStdString(finalCsv).toUtf8());
                file.close();
                QMessageBox::information(this, tr("Success"), tr("Your file was successfully exported"));
            }
        }
    }

    size_t MainWindow::FindSeparatorOrNewLine(std::string &str, const std::string &separator, const std::string &newLine, size_t &size) const noexcept
    {
        size_t pos = str.find(separator);
        if (pos == std::string::npos)
        {
            pos = str.find(newLine);
            size = newLine.size();
        }
        else
            size = separator.size();
        if (pos == std::string::npos && str != newLine && str != "")
        {
            str += newLine;
            return (FindSeparatorOrNewLine(str, separator, newLine, size));
        }
        return (pos);
    }

    void MainWindow::SaveFileInternal() const noexcept
    {
        QFile file(_saveStr);
        file.open(QIODevice::WriteOnly);
        file.write(_csvText.toPlainText().toUtf8());
        file.close();
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

    void MainWindow::LoadDataInfo() noexcept
    {
        _availableLineList.clear();
        const std::string &separator = EscapeString(_defaultSeparatorEdit.text().toStdString());
        const std::string &newLine = EscapeString(_defaultNewLineEdit.text().toStdString());
        const std::string &content = _csvText.toPlainText().toStdString();
        std::string firstLine = content.substr(0, content.find(newLine));
        size_t pos = 0;
        std::string token;
        size_t size;
        while ((pos = FindSeparatorOrNewLine(firstLine, separator, newLine, size)) != std::string::npos) {
            token = firstLine.substr(0, pos);
            _availableLineList.push_back(QString::fromStdString(token));
            firstLine.erase(0, pos + size);
        }
        UpdateColumnList();
    }

    void MainWindow::UpdateColumnList() noexcept
    {
        _columnSelection.clear();
        for (const QString &str : _availableLineList)
            _columnSelection.addItem(str);
    }

    void MainWindow::SetDataTable() noexcept
    {
        _csvTable.reset();
        std::vector<std::string> allLines = GetAllLines();
        if (allLines.size() == 1)
            return;
        const std::string &separator = EscapeString(_defaultSeparatorEdit.text().toStdString());
        const std::string &newLine = EscapeString(_defaultNewLineEdit.text().toStdString());
        QList<QString> header;
        size_t pos;
        int i = 0;
        std::string token;
        std::string content = _csvText.toPlainText().toStdString();
        std::string firstLine = content.substr(0, content.find(newLine));
        size_t size;
        while ((pos = FindSeparatorOrNewLine(firstLine, separator, newLine, size)) != std::string::npos) {
            token = firstLine.substr(0, pos);
            header.push_back(QString::fromStdString(token));
            firstLine.erase(0, pos + size);
            i++;
        }
        _csvTable.setRowCount(static_cast<int>(allLines.size() - 1));
        _csvTable.setColumnCount(header.size());
        _csvTable.setHorizontalHeaderLabels(header);
        for (size_t y = 1; y < allLines.size(); y++)
        {
            i = 0;
            while ((pos = FindSeparatorOrNewLine(allLines[y], separator, newLine, size)) != std::string::npos) {
                token = allLines[y].substr(0, pos);
                _csvTable.setItem(static_cast<int>(y - 1), i, new QTableWidgetItem(QString::fromStdString(token)));
                allLines[y].erase(0, pos + size);
                i++;
            }
        }
    }

    std::vector<std::string> MainWindow::GetAllLines() const noexcept
    {
        const std::string &newLine = EscapeString(_defaultNewLineEdit.text().toStdString());
        std::string content = _csvText.toPlainText().toStdString();
        std::string firstLine = content.substr(0, content.find(newLine));
        size_t pos = 0;
        std::string token;
        std::vector<std::string> allLines;
        while ((pos = content.find(newLine)) != std::string::npos) {
            token = content.substr(0, pos);
            allLines.push_back(token);
            content.erase(0, pos + newLine.size());
        }
        if (content != "")
            allLines.push_back(content);
        return (allLines);
    }
}
