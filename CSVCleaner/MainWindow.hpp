#ifndef MAINWINDOW_HPP
# define MAINWINDOW_HPP

# include <QMainWindow>
# include <QMenuBar>
# include <QPlainTextEdit>
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
        void resizeEvent(QResizeEvent *event) noexcept override;
        QMenu *_fileMenu, *_helpMenu;
        std::unique_ptr<QAction> _openAction, _quitAction, _aboutQtAction;
        QPlainTextEdit _csvText;

    private slots:
        void OpenFile();
        __attribute__((noreturn)) void ExitApp() const noexcept;
        void AboutQt() const noexcept;
    };
}

#endif // MAINWINDOW_HPP
