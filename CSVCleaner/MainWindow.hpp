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

namespace CSVCleaner
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow() noexcept;
        ~MainWindow() noexcept override = default;

    private:
        size_t FindSeparatorOrNewLine(std::string &str, const std::string &separator, const std::string &newLine, size_t &size) const noexcept;
        void SaveFileInternal() const noexcept;
        std::string EscapeString(std::string &&str) const noexcept;
        void LoadDataInfo() noexcept;
        void UpdateColumnList() noexcept;
        void SetDataTable() noexcept;
        std::vector<std::string> GetAllLines() const noexcept;
        QWidget _mainWidget;
        QMenu *_fileMenu, *_helpMenu;
        QGroupBox _previewBox, _configBox, _modifBox, _cleanBox;
        QGroupBox _defaultSeparatorBox, _defaultNewLineBox;
        QGroupBox _cleanOptionsBox;
        QTabWidget _previewTab;
        QVBoxLayout _previewLayout, _mainLayout;
        QGridLayout _modifLayout, _cleanLayout;
        QHBoxLayout _configLayout;
        QHBoxLayout _defaultSeparatorLayout, _defaultNewLineLayout;
        QHBoxLayout _cleanOptionsLayout;
        QLineEdit _defaultSeparatorEdit, _defaultNewLineEdit;
        std::unique_ptr<QAction> _openAction, _quitAction, _aboutQtAction, _saveAction, _saveAsAction;
        QPlainTextEdit _csvText;
        QTableWidget _csvTable;
        QComboBox _columnSelection;
        QLabel _selectedLineLabel;
        QList<QString> _availableLineList, _selectedLineList;
        QPushButton _selectedAdd, _selectedReset;
        QPushButton _selectedExport, _cleanStart;
        QCheckBox _showDupplicateCheck;
        QCheckBox _ignoreCaseCheck, _ignoreAccentsCheck, _ignorePunctuationCheck;
        QString _saveStr;

    private slots:
        void CleanColumns() noexcept;
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
    };
}

#endif // MAINWINDOW_HPP
