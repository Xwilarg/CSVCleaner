#ifndef CLEANWINDOW_HPP
# define CLEANWINDOW_HPP

# include <QMainWindow>
# include <QTableWidget>
# include <QGridLayout>
# include <QPushButton>
# include <memory>

namespace CSVCleaner
{
    class MainWindow;

    class CleanWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        CleanWindow(MainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                    const QList<QList<QString>> & allColumns, bool removeDupplicate) noexcept;
        CleanWindow(MainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                    QString &&modelFile) noexcept;
        ~CleanWindow() noexcept = default;

    private:
        CleanWindow(MainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef) noexcept;
        void Init() noexcept;
        void closeEvent(QCloseEvent *event) noexcept override;
        bool IsQTableWidgetItemNullOrEmpty(QTableWidgetItem *item) const noexcept;
        MainWindow &_parent;
        QWidget _mainWidget;
        QTabWidget _mainTab;
        QGridLayout _mainLayout;
        std::unique_ptr<CleanWindow>& _cleanWindowRef;
        QPushButton _export, _apply;
        QList<QTableWidget*> _tables;
        QList<QString> _allNames;
        const char _separator, _newLine;

    private slots:
        void Apply() noexcept;
        void Export() const noexcept;
    };
}

#endif // CLEANWINDOW_HPP
