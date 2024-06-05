#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <map>
#include <QPushButton>
#include "pairhash.h"
#include <utility>
#include "indexandcolor.h"
#include "qcustomplot.h"

#ifndef POINTS_LOG_H
#define POINTS_LOG_H


namespace Ui {

    class PointsLog : public QScrollArea {

        Q_OBJECT

    private:

        QWidget* parent_;
        QWidget* widget_;
        QVBoxLayout* layout_;
        std::unordered_map<std::pair<double, double>, QHBoxLayout*, pair_hash, pair_equal> data_points_ptrs_;
        std::unordered_map<std::pair<double, double>, QPushButton*, pair_hash, pair_equal> delete_button_ptrs_;
        std::unordered_map<std::pair<double, double>, QPushButton*, pair_hash, pair_equal> label_button_ptrs_;
        void delete_pair_of_buttons(QPushButton* first, QPushButton* second, QHBoxLayout* l);

    public:

        PointsLog(QWidget* parent = 0);
        void add_a_pair_of_buttons(std::pair<double, double> point, QCustomPlot* plot_ptr, std::unordered_map<std::pair<double, double>, IndexAndColor, pair_hash, pair_equal>* points_in_the_plot);
        void add_a_single_button(std::pair<double, double> point, QCustomPlot* plot_ptr, std::unordered_map<std::pair<double, double>, IndexAndColor, pair_hash, pair_equal>* points_in_the_plot_);
        void remove_certain_element(std::pair<double, double> point, bool surpress_update = false);
        void remove_all_elements();
        std::vector<std::pair<double, double>> get_all_current_data_points();
        virtual ~PointsLog() {;}
    
    };

}


#endif