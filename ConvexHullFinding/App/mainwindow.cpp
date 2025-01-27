#include "mainwindow.h"
#include "pointslog.h"
#include <memory>
#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QScrollArea>
#include <QPushButton>
#include <QIcon>
#include <QComboBox>
#include <QString>
#include <QObject>
#include <sstream>
#include <iostream>
#include <thread>
#include "graham.h"
#include <utility>
#include <functional>
#include <QMetaObject>


MainWindow::MainWindow() {
    int window_width = 800;
    int window_height = 800;
    this->algorithm_running_ = false;
    this->current_stack_ptr_ = std::make_shared<std::vector<std::pair<double, double>>>(0);
    this->window_ptr_ = std::make_unique<QWidget>(nullptr);
    this->window_ptr_->resize(window_width, window_height);

    QHBoxLayout* main_layout = new QHBoxLayout(this->window_ptr_.get());

    QVBoxLayout* left_layout = new QVBoxLayout(this->window_ptr_.get());
    Ui::PointsLog* all_points = new Ui::PointsLog(this->window_ptr_.get());
    this->all_points_ = all_points;
    QPushButton* clear_all_points_button = new QPushButton("Delete all points", this->window_ptr_.get());
    QObject::connect(clear_all_points_button, &QPushButton::clicked, [&](){clear_all_points();});
    QPushButton* set_all_to_blue_button = new QPushButton("Set all to blue", this->window_ptr_.get());
    QObject::connect(set_all_to_blue_button, &QPushButton::clicked, [&](){set_all_to_blue();});
    left_layout->addWidget(all_points);
    left_layout->addWidget(set_all_to_blue_button, 0);
    left_layout->addWidget(clear_all_points_button, 0);
    main_layout->addLayout(left_layout, 3);

    QVBoxLayout* central_layout = new QVBoxLayout(this->window_ptr_.get());
    
    QTextEdit* entry = new QTextEdit(this->window_ptr_.get());
    this->entry_ = entry;
    entry->setPlaceholderText("Please enter the data here...");
    central_layout->addWidget(entry, 1);

    QHBoxLayout* entry_button_layout = new QHBoxLayout(this->window_ptr_.get());
    QPushButton* add_points_button = new QPushButton("Add points", this->window_ptr_.get());
    QObject::connect(add_points_button, &QPushButton::clicked, [&](){add_points();});
    entry_button_layout->addWidget(add_points_button, 1);
    QPushButton* try_to_remove_points_button = new QPushButton("Try to remove points", this->window_ptr_.get());
    QObject::connect(try_to_remove_points_button, &QPushButton::clicked, [&](){try_to_remove_points();});
    entry_button_layout->addWidget(try_to_remove_points_button, 1);
    central_layout->addLayout(entry_button_layout, 0);

    QCustomPlot* plot = new QCustomPlot(this->window_ptr_.get());
    this->for_vis_.plot_ptr_ = plot;
    this->for_vis_.sleep_time_ = 0;
    this->for_vis_.should_halt_ = false;
    plot->xAxis->setRange(0, 5);
    plot->yAxis->setRange(0, 5);
    plot->xAxis->setVisible(true);
    plot->yAxis->setVisible(true);
    plot->xAxis->setLabel("x axis");
    plot->yAxis->setLabel("y axis");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    central_layout->addWidget(plot, 10);

    this->algorithm_thread_ = new AlgorithmThread(this);
    QObject::connect(this->algorithm_thread_, &QThread::finished, this->algorithm_thread_, [=](){;});
    
    QHBoxLayout* button_layout = new QHBoxLayout(this->window_ptr_.get());
    QPushButton* run_button = new QPushButton("Run", this->window_ptr_.get());
    QObject::connect(run_button, &QPushButton::clicked, [&](){
        if (this->algorithm_running_) return;
        this->points_in_ch_->remove_all_elements();
        std::function<void()> run_lambda = [&](){this->run();};
        this->algorithm_thread_->set_lambda(run_lambda);
        this->algorithm_thread_->start();
    });
    button_layout->addWidget(run_button, 1);
    QPushButton* run_and_vis_button = new QPushButton("Run and visualize", this->window_ptr_.get());
    QObject::connect(run_and_vis_button, &QPushButton::clicked, [&](){
        if (this->algorithm_running_) return;
        this->points_in_ch_->remove_all_elements();
        std::function<void()> run_lambda = [&](){this->run_and_vis();};
        this->algorithm_thread_->set_lambda(run_lambda);
        this->algorithm_thread_->start();
    });
    button_layout->addWidget(run_and_vis_button, 1);
    QPushButton* halt_or_resume_button = new QPushButton("Halt", this->window_ptr_.get());
    QObject::connect(halt_or_resume_button, &QPushButton::clicked, [&](){halt_or_resume();});
    button_layout->addWidget(halt_or_resume_button, 1);
    this->halt_or_resume_button_ = halt_or_resume_button;
    QPushButton* jump_button = new QPushButton("Skip to the final result", this->window_ptr_.get());
    QObject::connect(jump_button, &QPushButton::clicked, [&](){jump();});
    button_layout->addWidget(jump_button, 1);
    central_layout->addLayout(button_layout, 1);

    QScrollArea* instructions_frame = new QScrollArea(this->window_ptr_.get());
    QWidget* instruction_widget = new QWidget(instructions_frame);
    instructions_frame->setWidgetResizable(true);
    QVBoxLayout* instruction_layout = new QVBoxLayout(instruction_widget);
    instructions_frame->setWidget(instruction_widget);
    
    QLabel* instructions_label1 = new QLabel();
    const QString instructions_label_text1 = "- <b>In the text area at the top</b> type in the points data";
    instructions_label1->setText(instructions_label_text1);
    instruction_layout->addWidget(instructions_label1);
    
    QLabel* instructions_label2 = new QLabel();
    const QString instructions_label_text2 = "- <b>By pressing the corresponding button directly below the text entry</b>\ndecide what shall be done with entered area";                                
    instructions_label2->setText(instructions_label_text2);                                  
    instruction_layout->addWidget(instructions_label2);

    QLabel* instructions_label3 = new QLabel();
    const QString instructions_label_text3 = "- <b>To the left</b> points for which the convex hull shall be sought are displayed;\nyou can delete and hightlight those points by clicking the buttons corresponding to them";                                
    instructions_label3->setText(instructions_label_text3);                                  
    instruction_layout->addWidget(instructions_label3);

    QLabel* instructions_label4 = new QLabel();
    const QString instructions_label_text4 = "- <b>By using the combo box</b> decide what method shall be used to find the convex hull";                                
    instructions_label4->setText(instructions_label_text4);                                  
    instruction_layout->addWidget(instructions_label4);

    QLabel* instructions_label5 = new QLabel();
    const QString instructions_label_text5 = "- <b>Select Run</b> to perform the selected method and see the end results";                                
    instructions_label5->setText(instructions_label_text5);                                  
    instruction_layout->addWidget(instructions_label5);

    QLabel* instructions_label6 = new QLabel();
    const QString instructions_label_text6 = "- <b>Select Run and visualize</b> to start the algorithm and view consecutive steps";                                
    instructions_label6->setText(instructions_label_text6);                                  
    instruction_layout->addWidget(instructions_label6);

    QLabel* instructions_label7 = new QLabel();
    const QString instructions_label_text7 = "- <b>Pressing Halt</b> will stop the visualization until the same button is pressed again";                                
    instructions_label7->setText(instructions_label_text7);                                  
    instruction_layout->addWidget(instructions_label7);

    QLabel* instructions_label8 = new QLabel();
    const QString instructions_label_text8 = "- <b>Press Skip to the final result</b> if you wish to skip the rest of the visualization";                                
    instructions_label8->setText(instructions_label_text8);                                  
    instruction_layout->addWidget(instructions_label8);

    QLabel* instructions_label9 = new QLabel();
    const QString instructions_label_text9 = "- <b>To the right</b> points that make up the convex hull will be displayed in counter-clockwise order";                                
    instructions_label9->setText(instructions_label_text9);                                  
    instruction_layout->addWidget(instructions_label9);

    QLabel* instructions_label10 = new QLabel();
    const QString instructions_label_text10 = "- <b>Press Save results</b> to save the sorted list of the points that make up the convex hull to a file";                                
    instructions_label10->setText(instructions_label_text10);                                  
    instruction_layout->addWidget(instructions_label10);

    QLabel* instructions_label10a = new QLabel();
    const QString instructions_label_text10a = "- <b>Press Show current stack</b> to display found convex hull (or what was found up to a ceratin point at least)";                                
    instructions_label10->setText(instructions_label_text10a);                                  
    instruction_layout->addWidget(instructions_label10a);

    QLabel* instructions_label11 = new QLabel();
    const QString instructions_label_text11 = "- <b>Press point label buttons</b> to change their color in the plot";                                
    instructions_label11->setText(instructions_label_text11);                                  
    instruction_layout->addWidget(instructions_label11);

    QLabel* instructions_label12 = new QLabel();
    const QString instructions_label_text12 = "- <b>Press Set all to blue</b> to reset the color of points in the plot to blue";                                
    instructions_label12->setText(instructions_label_text12);                                  
    instruction_layout->addWidget(instructions_label12);

    central_layout->addWidget(instructions_frame, 3);                                    

    main_layout->addLayout(central_layout, 10);

    QVBoxLayout* right_layout = new QVBoxLayout(this->window_ptr_.get());
    Ui::PointsLog* points_in_ch = new Ui::PointsLog(this->window_ptr_.get());
    this->points_in_ch_ = points_in_ch;
    QPushButton* show_stack = new QPushButton("Show current stack", this->window_ptr_.get());
    QObject::connect(show_stack, &QPushButton::clicked, [&](){show_current_stack();});

    right_layout->addWidget(points_in_ch);
    right_layout->addWidget(show_stack, 0);
    main_layout->addLayout(right_layout, 3);

    this->window_ptr_->setLayout(main_layout);

}


MainWindow::~MainWindow() {;}


void MainWindow::launch() {
    this->window_ptr_->show();
}


void MainWindow::clear_all_points() {
    this->all_points_->remove_all_elements();
}


void MainWindow::run() {
    this->points_in_the_plot_.clear();
    this->algorithm_running_ = true;
    this->for_vis_.plot_ptr_->clearGraphs();
    this->for_vis_.plot_ptr_->clearItems();
    this->for_vis_.plot_ptr_->replot();
    this->for_vis_.should_halt_ = false;
    this->for_vis_.sleep_time_ = 0;
    this->change_halt_or_resume_button();    
        
    std::vector<std::pair<double, double>> all_current_data_points = this->all_points_->get_all_current_data_points();
    for (int i=0; i<all_current_data_points.size(); i++) {
        IndexAndColor index_and_color;
        index_and_color.color_ = Qt::blue;
        index_and_color.index_ = i;
        this->points_in_the_plot_[all_current_data_points[i]] = index_and_color;
    }

    this->current_stack_ptr_->clear();
    Graham_algorithm(all_current_data_points, &this->for_vis_, this->current_stack_ptr_);
    this->algorithm_running_ = false;

}


void MainWindow::run_and_vis() {
    this->points_in_the_plot_.clear();
    this->algorithm_running_ = true;
    this->for_vis_.plot_ptr_->clearGraphs();
    this->for_vis_.plot_ptr_->clearItems();
    this->for_vis_.plot_ptr_->replot();
    this->for_vis_.should_halt_ = false;
    this->for_vis_.sleep_time_ = 2;
    this->change_halt_or_resume_button();
        
    std::vector<std::pair<double, double>> all_current_data_points = this->all_points_->get_all_current_data_points();
    for (int i=0; i<all_current_data_points.size(); i++) {
        IndexAndColor index_and_color;
        index_and_color.color_ = Qt::blue;
        index_and_color.index_ = i;
        this->points_in_the_plot_[all_current_data_points[i]] = index_and_color;
    }

    this->current_stack_ptr_->clear();
    Graham_algorithm(all_current_data_points, &this->for_vis_, this->current_stack_ptr_);
    this->algorithm_running_ = false;
    
}


void MainWindow::change_halt_or_resume_button() {
    if (! this->for_vis_.should_halt_) {
        this->halt_or_resume_button_->setText("Halt");
    }
    else {
        this->halt_or_resume_button_->setText("Resume");
    }
}


void MainWindow::halt_or_resume() {
    if (this->algorithm_running_) {
        if (! this->for_vis_.should_halt_) {
            this->for_vis_.should_halt_ = true;
        }
        else {
            this->for_vis_.should_halt_ = false;
        }
        this->change_halt_or_resume_button();
    }
}


void MainWindow::jump() {
    if (this->algorithm_running_) {
        this->for_vis_.should_halt_ = false;
        this->for_vis_.sleep_time_ = 0;
        this->change_halt_or_resume_button();
    }
}


void MainWindow::show_current_stack() {
    if ((! this->algorithm_running_) || this->for_vis_.should_halt_) {
        this->points_in_ch_->remove_all_elements();
        for (int i=0; i<this->current_stack_ptr_->size(); i++) {
            this->points_in_ch_->add_a_single_button(this->current_stack_ptr_->at(i), this->for_vis_.plot_ptr_, &this->points_in_the_plot_);
        }
    }
}


std::vector<std::pair<double, double>> MainWindow::get_points_from_entry() {
    QString qs_entered_data = this->entry_->toPlainText();
    std::string entered_data = qs_entered_data.toStdString();
    std::stringstream stream(entered_data);
    std::vector<std::string> point_strings;
    std::string extracted;

    while(std::getline(stream, extracted)) {
        point_strings.push_back(extracted);
    }

    std::vector<std::pair<double, double>> points_from_entry;
    char unused;
    for (std::string point_string : point_strings) {
        std::pair<double, double> new_point;
        std::istringstream new_stream(point_string);
        new_stream >> unused >> new_point.first >> unused >> new_point.second >> unused;
        points_from_entry.push_back(new_point);
    }

    return points_from_entry;
}


void MainWindow::add_points() {
    std::vector<std::pair<double, double>> points_from_entry = get_points_from_entry();
    for (auto point : points_from_entry) {
        this->all_points_->add_a_pair_of_buttons(point, this->for_vis_.plot_ptr_, &this->points_in_the_plot_);
    }
}


void MainWindow::try_to_remove_points() {
    std::vector<std::pair<double, double>> points_from_entry = get_points_from_entry();
    for (auto point : points_from_entry) {
        this->all_points_->remove_certain_element(point);
    }
}


void MainWindow::set_all_to_blue() {
    QCPScatterStyle blue_scatter_style;
    blue_scatter_style.setShape(QCPScatterStyle::ssCircle);
    blue_scatter_style.setSize(6);
    blue_scatter_style.setBrush(Qt::blue);
    for (const auto& pair : this->points_in_the_plot_) {
        this->points_in_the_plot_[pair.first].color_ = Qt::blue;
        this->for_vis_.plot_ptr_->graph(pair.second.index_)->setScatterStyle(blue_scatter_style);
        this->for_vis_.plot_ptr_->graph(pair.second.index_)->setPen(QPen(Qt::blue));
    }
    this->for_vis_.plot_ptr_->replot();
}