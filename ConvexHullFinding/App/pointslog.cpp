#include "pointslog.h"
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>
#include <iostream>
#include <QObject>
#include <QPushButton>
#include <sstream>
#include <iomanip>


std::string double_to_string (double number) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(10) << number;
    std::string str = oss.str();
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    if (str.back() == '.') {
        str.pop_back();
    }

    return str;
}


QColor get_next_color (QColor current_color) {
    if (current_color == Qt::blue) {
        return Qt::red;
    }
    else if (current_color == Qt::red) {
        return Qt::darkYellow;
    }
    else if (current_color == Qt::darkYellow) {
        return Qt::darkGreen;
    }
    else if (current_color == Qt::darkGreen) {
        return Qt::darkMagenta;
    }
    else if (current_color == Qt::darkMagenta) {
        return Qt::darkCyan;
    }
    else {
        return Qt::blue;
    }
}


void Ui::PointsLog::delete_pair_of_buttons (QPushButton* first, QPushButton* second, QHBoxLayout* l) {
    if (first) {
        first->hide();
        second->deleteLater();
    }
    if (second) {
        second->hide();
        first->deleteLater();
    }
    if (l) {
        l->deleteLater();
    }
    this->layout_->update();
}


Ui::PointsLog::PointsLog (QWidget* parent) : 
    QScrollArea(parent) {
    QWidget* widget = new QWidget(this);
    this->setWidgetResizable(true);
    this->parent_ = parent;
    this->widget_ = widget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    this->layout_ = layout;
    widget->setLayout(layout);
    this->setWidget(widget);
}


void Ui::PointsLog::remove_certain_element (std::pair<double, double> point, bool surpress_update) {
    if (this->data_points_ptrs_.find(point) != this->data_points_ptrs_.end()) {
        QHBoxLayout* to_remove = this->data_points_ptrs_[point];
        if (this->delete_button_ptrs_.find(point) != this->delete_button_ptrs_.end()) {
            QObject::disconnect(this->delete_button_ptrs_[point], &QPushButton::clicked, nullptr, nullptr);
        }
        QObject::disconnect(this->label_button_ptrs_[point], &QPushButton::clicked, nullptr, nullptr);
        QLayoutItem* item;
        while ((item = to_remove->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
        to_remove->deleteLater();
        this->data_points_ptrs_.erase(point);
        this->delete_button_ptrs_.erase(point);
        this->label_button_ptrs_.erase(point);
        if (! surpress_update) this->layout_->update();
    }

}


void Ui::PointsLog::remove_all_elements () {
    std::vector<std::pair<double, double>> keys;
    for (const auto& pair : this->data_points_ptrs_) {
        keys.push_back(pair.first);
    }
    for (const auto& point : keys) {
        this->remove_certain_element(point, true);
    }
    this->layout_->update();
}


void Ui::PointsLog::add_a_pair_of_buttons(std::pair<double, double> point, QCustomPlot* plot_ptr, std::unordered_map<std::pair<double, double>, IndexAndColor, pair_hash, pair_equal>* points_in_the_plot) {
    if (this->data_points_ptrs_.find(point) == this->data_points_ptrs_.end()) {    
        std::string point_label = "(" + double_to_string(point.first) + ", " + double_to_string(point.second) + ")";
        QString q_point_label = QString::fromStdString(point_label);
        QPushButton* label_button = new QPushButton(q_point_label, this->widget_);
        QPushButton* delete_button = new QPushButton("Delete", this->widget_);
        QHBoxLayout* button_pair = new QHBoxLayout(this->widget_);
        button_pair->addWidget(label_button, 1);
        button_pair->addWidget(delete_button, 1);
        this->layout_->addLayout(button_pair, 1);
        button_pair->setAlignment(Qt::AlignTop);
        QObject::connect(label_button, &QPushButton::clicked, [=](){
            if (points_in_the_plot->find(point) != points_in_the_plot->end()) {
                QColor new_color = get_next_color((*points_in_the_plot)[point].color_);
                (*points_in_the_plot)[point].color_ = new_color;
                QCPScatterStyle scatter_style;
                scatter_style.setShape(QCPScatterStyle::ssCircle);
                scatter_style.setSize(6);
                scatter_style.setBrush((*points_in_the_plot)[point].color_);
                plot_ptr->graph((*points_in_the_plot)[point].index_)->setScatterStyle(scatter_style);
                plot_ptr->graph((*points_in_the_plot)[point].index_)->setPen(QPen((*points_in_the_plot)[point].color_));
                plot_ptr->replot();
            }
        });
        QObject::connect(delete_button, &QPushButton::clicked, [=](){
            delete_pair_of_buttons(label_button, delete_button, button_pair);
            QObject::disconnect(delete_button, &QPushButton::clicked, nullptr, nullptr);
            QObject::disconnect(label_button, &QPushButton::clicked, nullptr, nullptr);
            this->data_points_ptrs_.erase(point);
            this->delete_button_ptrs_.erase(point);
            this->label_button_ptrs_.erase(point);
        }); 
        this->data_points_ptrs_[point] = button_pair;
        this->delete_button_ptrs_[point] = delete_button;
        this->label_button_ptrs_[point] = label_button;
    }        
}


void Ui::PointsLog::add_a_single_button(std::pair<double, double> point, QCustomPlot* plot_ptr, std::unordered_map<std::pair<double, double>, IndexAndColor, pair_hash, pair_equal>* points_in_the_plot) {
    if (this->data_points_ptrs_.find(point) == this->data_points_ptrs_.end()) {    
        std::string point_label = "(" + double_to_string(point.first) + ", " + double_to_string(point.second) + ")";
        QString q_point_label = QString::fromStdString(point_label);
        QPushButton* label_button = new QPushButton(q_point_label, this->widget_);
        QHBoxLayout* button_hlayout = new QHBoxLayout(this->widget_);
        button_hlayout->addWidget(label_button, 1);
        this->layout_->addLayout(button_hlayout, 1);
        button_hlayout->setAlignment(Qt::AlignTop);
        QObject::connect(label_button, &QPushButton::clicked, [=](){
            if (points_in_the_plot->find(point) != points_in_the_plot->end()) {
                QColor new_color = get_next_color((*points_in_the_plot)[point].color_);
                (*points_in_the_plot)[point].color_ = new_color;
                QCPScatterStyle scatter_style;
                scatter_style.setShape(QCPScatterStyle::ssCircle);
                scatter_style.setSize(6);
                scatter_style.setBrush((*points_in_the_plot)[point].color_);
                plot_ptr->graph((*points_in_the_plot)[point].index_)->setScatterStyle(scatter_style);
                plot_ptr->graph((*points_in_the_plot)[point].index_)->setPen(QPen((*points_in_the_plot)[point].color_));
                plot_ptr->replot();
            }
        }); 
        this->data_points_ptrs_[point] = button_hlayout;
        this->label_button_ptrs_[point] = label_button;
    }  
}


std::vector<std::pair<double, double>> Ui::PointsLog::get_all_current_data_points () {
    std::vector<std::pair<double, double>> all_current_data_points;
    for (const auto& pair : this->data_points_ptrs_) {
        all_current_data_points.push_back(pair.first);
    }
    return all_current_data_points;
}