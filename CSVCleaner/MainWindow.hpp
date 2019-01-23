#ifndef MAINWINDOW_HPP
# define MAINWINDOW_HPP

# include <QMainWindow>
# include <QMenuBar>
# include <QPlainTextEdit>
# include <QGroupBox>
# include <QGridLayout>
# include <QTableView>
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
        QMenu *_fileMenu, *_helpMenu;
        QGroupBox _previewBox;
        QTabWidget _previewTab;
        QGridLayout _previewLayout;
        std::unique_ptr<QAction> _openAction, _quitAction, _aboutQtAction;
        QPlainTextEdit _csvText;
        QTableView _csvTable;

    private slots:
        void OpenFile();
        __attribute__((noreturn)) void ExitApp() const noexcept;
        void AboutQt() const noexcept;
    };
}

#endif // MAINWINDOW_HPP
