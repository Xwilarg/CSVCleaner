#ifndef MAINWINDOW_HPP
# define MAINWINDOW_HPP

# include <QMainWindow>
# include <QMenuBar>
# include <QPlainTextEdit>
# include <QGroupBox>
# include <QGridLayout>
# include <QTableWidget>
# include <QLineEdit>
# include <QComboBox>
# include <QLabel>
# include <QPushButton>
# include <QCheckBox>
# include <memory>
# include "CleanWindow.hpp"

namespace CSVCleaner
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow() noexcept;
        ~MainWindow() noexcept = default;
        void ApplyTable(QList<QString> &&name, QList<std::map<QString, QString>> &&allItems) noexcept;

    private:
        QString CleanWord(QString &&str) const noexcept;
        size_t FindSeparatorOrNewLine(std::string &str, const std::string &separator, const std::string &newLine, size_t &size) const noexcept;
        void SaveFileInternal() const noexcept;
        std::string UnescapeString(std::string &&str) const noexcept;
        void LoadDataInfo() noexcept;
        void UpdateColumnList() noexcept;
        void SetDataTable() noexcept;
        std::string RemoveNewLine(std::string &&str, bool forceClean) const noexcept;
        std::string GetFirstLine() const noexcept;
        std::vector<std::string> GetAllLines(bool forceClean = false) const noexcept;
        bool IsEmpty(const std::string &str) const noexcept;
        QWidget _mainWidget;
        QMenu *_fileMenu, *_helpMenu;
        QGroupBox _previewBox, _configBox, _modifBox, _cleanBox;
        QGroupBox _defaultSeparatorBox, _defaultNewLineBox;
        QGroupBox _cleanOptionsBox;
        QTabWidget _previewTab;
        QVBoxLayout _previewLayout, _mainLayout;
        QGridLayout _modifLayout, _cleanLayout;
        QHBoxLayout _configLayout;
        QVBoxLayout _defaultSeparatorLayout, _defaultNewLineLayout;
        QHBoxLayout _cleanOptionsLayout;
        QLineEdit _defaultSeparatorEdit, _defaultNewLineEdit;
        std::unique_ptr<QAction> _openAction, _quitAction, _aboutQtAction, _saveAction, _saveAsAction, _refreshAction;
        QPlainTextEdit _csvText;
        QTableWidget _csvTable;
        QComboBox _columnSelection;
        QLabel _selectedLineLabel;
        QList<QString> _availableLineList;
        QList<std::pair<QString, int>> _selectedLineList;
        QPushButton _selectedAdd, _selectedReset;
        QPushButton _selectedExport, _cleanStart, _cleanImport;
        QPushButton _cleanAll;
        QCheckBox _showDupplicateCheck;
        QCheckBox _ignoreCaseCheck, _ignoreAccentsCheck, _ignorePunctuationCheck;
        QCheckBox _ignoreNewLine;
        QString _saveStr;
        std::unique_ptr<CleanWindow> _cleanWindow;

    private slots:
        void RefreshProgram() noexcept;
        void CleanColumns() noexcept;
        void ImportClean() noexcept;
        void SaveFile() const noexcept;
        void SaveFileAs() noexcept;
        void OpenFile() noexcept;
        __attribute__((noreturn)) void ExitApp() const noexcept;
        void AboutQt() const noexcept;
        void OnTableLoad(int index) noexcept;
        void AddElement() noexcept;
        void ResetElements() noexcept;
        void ExportElements() noexcept;
        void ShowDupplicateStateChanged(bool state) noexcept;
        void NewLineTextChanged(const QString &str) noexcept;
        void CleanRawCsv() noexcept;
    };
}

#endif // MAINWINDOW_HPP
