#include "qcustomplot.h"
#include "pointslog.h"
#include "forvis.h"
#include <QMainWindow>
#include <QWidget>
#include <memory>
#include <QScrollArea>
#include <QString>
#include <QObject>
#include <map>
#include <cassert>
#include "graham.h"
#include "indexandcolor.h"
#include <utility>
#include "algorithmthread.h"
#include <QObject>
#include <QApplication>
#include <QDebug>
#include <QMetaObject>
#include <atomic>


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


namespace Ui {
    class MainWindow;
}


class MainWindow : public QObject {

    Q_OBJECT

public:

    MainWindow();
    ~MainWindow();
    void launch();

private:

    std::unique_ptr<QWidget> window_ptr_;
    QTextEdit* entry_;
    struct ForVis for_vis_;
    Ui::PointsLog* all_points_;
    Ui::PointsLog* points_in_ch_; 
    std::shared_ptr<std::vector<std::pair<double, double>>> current_stack_ptr_;
    QPushButton* halt_or_resume_button_;
    bool algorithm_running_;
    std::unordered_map<std::pair<double, double>, IndexAndColor, pair_hash, pair_equal> points_in_the_plot_;
    AlgorithmThread* algorithm_thread_;
    
    void clear_all_points();
    void run();
    void run_and_vis();
    void halt_or_resume();
    void change_halt_or_resume_button();
    void jump();
    void show_current_stack();
    void add_points();
    void try_to_remove_points();
    void set_all_to_blue();
    std::vector<std::pair<double, double>> get_points_from_entry();

};


#endif