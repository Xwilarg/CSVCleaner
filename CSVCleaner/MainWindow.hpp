#ifndef MAINWINDOW_HPP
# define MAINWINDOW_HPP

# include <QMainWindow>

namespace CSVCleaner
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow() noexcept;
        ~MainWindow() noexcept;
    };
}

#endif // MAINWINDOW_HPP
