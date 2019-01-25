#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include "MainWindow.hpp"

namespace CSVCleaner
{
    MainWindow::MainWindow() noexcept
        : QMainWindow(nullptr), _mainWidget(this),
          _fileMenu(menuBar()->addMenu(tr("File"))), _helpMenu(menuBar()->addMenu(tr("?"))),
          _previewBox(tr("Preview"), &_mainWidget), _configBox(tr("Configuration"), &_mainWidget),
          _modifBox(tr("Modification"), &_mainWidget), _cleanBox(tr("Cleaning"), &_mainWidget),
          _defaultSeparatorBox(tr("Default separator"), &_configBox), _defaultNewLineBox(tr("Default newline"), &_previewBox),
          _cleanOptionsBox(&_cleanBox),
          _previewTab(&_previewBox),
          _previewLayout(&_previewBox), _mainLayout(&_mainWidget), _modifLayout(&_modifBox),
          _cleanLayout(&_cleanBox),
          _configLayout(&_configBox), _defaultSeparatorLayout(&_defaultSeparatorBox), _defaultNewLineLayout(&_defaultNewLineBox),
          _cleanOptionsLayout(&_cleanOptionsBox),
          _defaultSeparatorEdit(&_defaultSeparatorBox), _defaultNewLineEdit(&_defaultNewLineBox),
          _openAction(std::make_unique<QAction>(tr("Open"), this)), _quitAction(std::make_unique<QAction>(tr("Quit"), this)),
          _aboutQtAction(std::make_unique<QAction>(tr("About Qt"), this)), _saveAction(std::make_unique<QAction>(tr("Save"), this)),
          _saveAsAction(std::make_unique<QAction>(tr("Save as..."), this)), _refreshAction(std::make_unique<QAction>(tr("Reload"), this)),
          _csvText(&_previewTab), _csvTable(&_previewTab),
          _columnSelection(&_modifBox), _selectedLineLabel(tr("Selected columns:\n"), &_modifBox),
          _availableLineList(), _selectedLineList(),
          _selectedAdd(tr("Add"), &_modifBox), _selectedReset(tr("Refresh"), &_modifBox),
          _selectedExport(tr("Export")), _cleanStart(tr("Start"), &_cleanBox),
          _showDupplicateCheck(tr("Show dupplicates"), &_cleanBox),
          _ignoreCaseCheck(tr("Ignore case"), &_cleanBox), _ignoreAccentsCheck(tr("Ignore accents"), &_cleanBox),
          _ignorePunctuationCheck(tr("Ignore punctuation"), &_cleanBox),
          _saveStr(""), _cleanWindow(nullptr)
    {
        constexpr int xSize = 600, ySize = 600;
        setWindowTitle("CSVCleaner");
        resize(xSize, ySize);

        // Set menu
        _refreshAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
        _openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
        _saveAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
        _saveAsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
        _fileMenu->addAction(_refreshAction.get());
        _fileMenu->addAction(_saveAction.get());
        _fileMenu->addAction(_saveAsAction.get());
        _fileMenu->addAction(_openAction.get());
        _fileMenu->addAction(_quitAction.get());
        _helpMenu->addAction(_aboutQtAction.get());

        // Set layout
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
        _modifLayout.addWidget(&_cleanBox, 4, 0, 1, 0);
        _cleanLayout.addWidget(&_cleanStart);
        _cleanLayout.addWidget(&_showDupplicateCheck);
        _cleanLayout.addWidget(&_cleanOptionsBox);
        _cleanOptionsLayout.addWidget(&_ignoreCaseCheck);
        _cleanOptionsLayout.addWidget(&_ignoreAccentsCheck);
        _cleanOptionsLayout.addWidget(&_ignorePunctuationCheck);

        // Modify widgets
        _previewTab.addTab(&_csvText, tr("Raw"));
        _previewTab.addTab(&_csvTable, tr("Table"));
        _csvText.setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
        _csvTable.setEditTriggers(QAbstractItemView::NoEditTriggers);
        _defaultSeparatorEdit.setText(";");
        _defaultNewLineEdit.setText("\\n");

        // Set connections
        connect(_refreshAction.get(), SIGNAL(triggered()), this, SLOT(RefreshProgram()));
        connect(_saveAction.get(), SIGNAL(triggered()), this, SLOT(SaveFile()));
        connect(_saveAsAction.get(), SIGNAL(triggered()), this, SLOT(SaveFileAs()));
        connect(_openAction.get(), SIGNAL(triggered()), this, SLOT(OpenFile()));
        connect(_quitAction.get(), SIGNAL(triggered()), this, SLOT(ExitApp()));
        connect(_aboutQtAction.get(), SIGNAL(triggered()), this, SLOT(AboutQt()));
        connect(&_previewTab, SIGNAL(currentChanged(int)), this, SLOT(OnTableLoad(int)));
        connect(&_selectedAdd, SIGNAL(clicked()), this, SLOT(AddElement()));
        connect(&_selectedReset, SIGNAL(clicked()), this, SLOT(ResetElements()));
        connect(&_selectedExport, SIGNAL(clicked()), this, SLOT(ExportElements()));
        connect(&_cleanStart, SIGNAL(clicked()), this, SLOT(CleanColumns()));
        connect(&_showDupplicateCheck, SIGNAL(clicked(bool)), this, SLOT(ShowDupplicateStateChanged(bool)));
    }

    /// Refresh program (useful when user change config)
    void MainWindow::RefreshProgram() noexcept
    {
        LoadDataInfo();
    }

    /// "Clean Start" command
    void MainWindow::CleanColumns() noexcept
    {
        if (_selectedLineList.size() == 0)
            QMessageBox::warning(this, tr("Error"), tr("There is no column selected"));
        else if (_cleanWindow != nullptr)
            QMessageBox::warning(this, tr("Error"), tr("A window is already opened"));
        else
        {
            // We get all columns that user selected
           /*const std::string &separator = UnescapeString(_defaultSeparatorEdit.text().toStdString());
            const std::string &newLine = UnescapeString(_defaultNewLineEdit.text().toStdString());
            std::vector<std::string> allLines = GetAllLines();
            QList<QList<QString>> finalLines;
            int i = -1;
            QList<int> allIds = GetIds(allLines[0]);
            std::string token;
            size_t size, pos;
            for (std::string str : allLines)
            {
                if (i++ == -1)
                    continue;
                i = 0;
                QList<QString> tmpList;
                while ((pos = FindSeparatorOrNewLine(str, separator, newLine, size)) != std::string::npos) {
                    token = str.substr(0, pos);
                    if (allIds.contains(i))
                        tmpList.push_back(QString::fromStdString(token));
                    str.erase(0, pos + size);
                    i++;
                }
                finalLines.push_back(std::move(tmpList));
            }

            // We create a new window (see CleanWindow)
            //_cleanWindow = std::make_unique<CleanWindow>(this, _cleanWindow,
            //                    _selectedLineList, finalLines[0]);
            _cleanWindow->show();
            */
        }
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
            QByteArray arr = file.readAll();
            QTextCodec *codec = QTextCodec::codecForUtfText(arr, QTextCodec::codecForName("System"));
            _csvText.document()->setPlainText(QString(codec->toUnicode(arr)));
            file.close();
            RefreshProgram();
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

    /// When user go in "Table" tab we reload it (in case he changed things in the raw tab)
    void MainWindow::OnTableLoad(int index) noexcept
    {
        if (index == 1)
            SetDataTable();
    }

    /// When player add a column
    void MainWindow::AddElement() noexcept
    {
        const QString &element = _columnSelection.currentText();
        if (element != "")
        {
            _selectedLineLabel.setText(_selectedLineLabel.text() + element + '\n');

            // Get column id
            std::string firstLine = GetFirstLine();
            std::string token;
            size_t pos, size;
            const std::string &separator = UnescapeString(_defaultSeparatorEdit.text().toStdString());
            const std::string &newLine = UnescapeString(_defaultNewLineEdit.text().toStdString());
            int i = 0;
            while ((pos = FindSeparatorOrNewLine(firstLine, separator, newLine, size)) != std::string::npos) {
                token = firstLine.substr(0, pos);
                if (QString::fromStdString(token) == element)
                {
                    _selectedLineList.push_back(std::pair<QString, int>(element, i));
                    _availableLineList.removeOne(element);
                }
                firstLine.erase(0, pos + size);
                i++;
            }
            UpdateColumnList();
        }
    }

    /// Reset all columns
    void MainWindow::ResetElements() noexcept
    {
        _selectedLineLabel.setText(tr("Selected columns:\n"));
        _selectedLineList.clear();
        LoadDataInfo();
    }

    /// Export chosen columns
    void MainWindow::ExportElements() noexcept
    {
        if (_selectedLineList.size() == 0)
            QMessageBox::warning(this, tr("Error"), tr("There is no column selected"));
        else
        {
            QString path(QFileDialog::getSaveFileName(this, tr("Save As"), "", tr("CSV (*.csv);;All Files (*)")));
            if (path != "")
            {
                QFile file(path);
                file.open(QIODevice::WriteOnly);
                std::vector<std::string> allLines = GetAllLines();
                const std::string &separator = UnescapeString(_defaultSeparatorEdit.text().toStdString());
                const std::string &newLine = UnescapeString(_defaultNewLineEdit.text().toStdString());
                size_t pos;
                int i = 0;
                std::string token;

                // CSV header
                size_t size;
                std::string finalCsv = "";
                bool isFirst = true;
                for (const auto &str : _selectedLineList)
                {
                    if (!isFirst)
                        finalCsv += separator;
                    isFirst = false;
                    finalCsv += str.first.toStdString();
                }
                finalCsv += newLine;

                // CSV body (using previous ids)
                for (size_t y = 1; y < allLines.size(); y++)
                {
                    isFirst = true;
                    for (int z = 0; z < _selectedLineList.size(); z++)
                    {
                        i = 0;
                        std::string tmp = allLines[y];
                        while ((pos = FindSeparatorOrNewLine(tmp, separator, newLine, size)) != std::string::npos) {
                            token = tmp.substr(0, pos);
                            qDebug() << i << " == "<< _selectedLineList[z].second << "  "<< QString::fromStdString( token);
                            if (_selectedLineList[z].second == i)
                            {
                                if (!isFirst)
                                    finalCsv += separator;
                                finalCsv += token;
                                isFirst = false;
                                break;
                            }
                            tmp.erase(0, pos + size);
                            i++;
                        }
                    }
                    finalCsv += newLine;
                }
                file.write(QString::fromStdString(finalCsv).toUtf8());
                file.close();
                QMessageBox::information(this, tr("Success"), tr("Your file was successfully exported"));
            }
        }
    }

    /// Find next separator or new line in a string
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

    /// Called by Save and SaveAs
    void MainWindow::SaveFileInternal() const noexcept
    {
        QFile file(_saveStr);
        file.open(QIODevice::WriteOnly);
        file.write(_csvText.toPlainText().toUtf8());
        file.close();
    }

    /// Unescape a string (\\n --> \n)
    std::string MainWindow::UnescapeString(std::string &&str) const noexcept
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

    /// Update _columnSelection drop down list with raw CSV
    void MainWindow::LoadDataInfo() noexcept
    {
        _availableLineList.clear();
        const std::string &separator = UnescapeString(_defaultSeparatorEdit.text().toStdString());
        const std::string &newLine = UnescapeString(_defaultNewLineEdit.text().toStdString());
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

    /// Update _columnSelection drop down list with current items
    void MainWindow::UpdateColumnList() noexcept
    {
        _columnSelection.clear();
        for (const QString &str : _availableLineList)
            _columnSelection.addItem(str);
    }

    /// Update CSV table
    void MainWindow::SetDataTable() noexcept
    {
        _csvTable.reset();
        std::vector<std::string> allLines = GetAllLines();
        if (allLines.size() == 1)
            return;
        const std::string &separator = UnescapeString(_defaultSeparatorEdit.text().toStdString());
        const std::string &newLine = UnescapeString(_defaultNewLineEdit.text().toStdString());
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

    /// Get first line of raw CSV
    std::string MainWindow::GetFirstLine() const noexcept
    {
        const std::string &newLine = UnescapeString(_defaultNewLineEdit.text().toStdString());
        std::string content = _csvText.toPlainText().toStdString();
        return (content.substr(0, content.find(newLine)));
    }

    /// Get all lines of raw CSV
    std::vector<std::string> MainWindow::GetAllLines() const noexcept
    {
        const std::string &newLine = UnescapeString(_defaultNewLineEdit.text().toStdString());
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

    void MainWindow::ShowDupplicateStateChanged(bool state) noexcept
    {
        _ignoreCaseCheck.setEnabled(!state);
        _ignoreAccentsCheck.setEnabled(!state);
        _ignorePunctuationCheck.setEnabled(!state);
    }
}
