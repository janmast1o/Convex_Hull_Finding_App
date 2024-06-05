#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <QVector>
#include <thread>
#include "graham.h"
#include "forvis.h"


double epsilon = std::pow(10,-6);
double x_min, x_max, y_min, y_max;


void act_accordingly_to_for_vis (struct ForVis* for_vis) {
    std::this_thread::sleep_for(std::chrono::seconds(for_vis->sleep_time_));
    while (for_vis->should_halt_) {;}
}


void scatter_initial_points (std::vector<std::pair<double, double>> &P, struct ForVis* for_vis) {
    QCPScatterStyle scatter_style;
    scatter_style.setShape(QCPScatterStyle::ssCircle);
    scatter_style.setSize(6);
    scatter_style.setBrush(Qt::blue);

    x_min = P[0].first;
    x_max = P[0].second;
    y_min = P[0].second;
    y_max = P[1].second;

    for (int i=1; i<P.size(); i++) {
        if (P[i].first < x_min) x_min = P[i].first;
        if (P[i].first > x_max) x_max = P[i].first;
        if (P[i].second < y_min) y_min = P[i].second;
        if (P[i].second > y_max) y_max = P[i].second;
    }
    
    for (int i=0; i<P.size(); i++) {
        for_vis->plot_ptr_->addGraph();
        for_vis->plot_ptr_->graph(i)->setScatterStyle(scatter_style);
        for_vis->plot_ptr_->graph(i)->setLineStyle(QCPGraph::lsNone);
        for_vis->plot_ptr_->graph(i)->setData(QVector<double>({P[i].first}), QVector<double>({P[i].second}));
        for_vis->plot_ptr_->graph(i)->setPen(QPen(Qt::blue));
    }

    for_vis->plot_ptr_->xAxis->setRange(x_min-1, x_max+1);
    for_vis->plot_ptr_->yAxis->setRange(y_min-1, y_max+1);
    for_vis->plot_ptr_->replot();
}


void add_line_segment (struct ForVis* for_vis, std::pair<double, double> a, std::pair<double, double> b, int* index) {
    for_vis->plot_ptr_->addGraph();
    for_vis->plot_ptr_->graph(*index)->setData(QVector<double>({a.first, b.first}), QVector<double>({a.second, b.second}));
    for_vis->plot_ptr_->graph(*index)->setPen(QPen(Qt::blue));
    (*index)++;
    for_vis->plot_ptr_->replot();
}


void remove_last_line_segment (struct ForVis* for_vis, int* index) {
    for_vis->plot_ptr_->removeGraph((*index)-1);
    (*index)--;
    for_vis->plot_ptr_->replot();
}


double compare (std::pair<double ,double> base, std::pair<double ,double> first, std::pair<double ,double> second) {
    double a_x = base.first;
    double a_y = base.second;
    double b_x = first.first;
    double b_y = first.second;
    double c_x = second.first;
    double c_y = second.second;

    double orient = (a_x-c_x)*(b_y-c_y) - (a_y-c_y)*(b_x-c_x);
    return orient;
}


double calculate_sq_distance_from (std::pair<double ,double> base, std::pair<double ,double> point) {
    return std::pow((base.first - point.first), 2) + std::pow((base.second - point.second), 2);
}


void sort_points_for_Graham (std::vector<std::pair<double, double>> &A, std::pair<double, double> lowest_point) {

    int points_size = A.size();

    std::function<void(std::vector<std::pair<double, double>>&, int, int)> merge;
    std::vector<std::pair<double, double>>* add_vec = new std::vector<std::pair<double, double>>(points_size); 
    merge = [lowest_point, &add_vec](std::vector<std::pair<double, double>> &A, int start, int stop) {       
        int a = start;
        int mid = (stop-start)/2 + start;
        int b = mid+1;
        int index = 0;
        double o;

        while (a <= mid && b <= stop) {
            o = compare(lowest_point, A[a], A[b]);
            if (std::abs(o) <= epsilon) {
                if (calculate_sq_distance_from(lowest_point, A[a]) < calculate_sq_distance_from(lowest_point, A[b])) {
                    (*add_vec)[index] = A[a];
                    a += 1;
                }
                else {
                    (*add_vec)[index] = A[b];
                    b += 1;
                } 
            }
            else if (o > epsilon) {
                (*add_vec)[index] = A[a];
                a += 1;
            }  
            else {
                (*add_vec)[index] = A[b];
                b += 1;
            }
            index += 1;
        }

        int x;
        int y;
        if (a == mid+1) {
            x = b;
            y = stop;
        }
        else {
            x = a;
            y = mid;
        }

        while (x <= y) {
            (*add_vec)[index] = A[x];
            x += 1;
            index += 1;
        }

        for (int i=start ; i<=stop; i++) {
            A[i] = (*add_vec)[i-start];
        }
    };

    std::function<void(std::vector<std::pair<double, double>>&, int, int)> internal_mergesort;
    internal_mergesort = [&internal_mergesort, &merge](std::vector<std::pair<double, double>> &A, int start, int stop){   
        if (stop-start >= 2) {
            int mid = ((stop-start)/2) + start;
            internal_mergesort(A, start, mid);
            internal_mergesort(A, mid+1, stop);
        }
        merge(A, start, stop);
    };

    internal_mergesort(A, 1, points_size-1);
    delete add_vec;
}


std::vector<std::pair<double, double>> remove_deg_duplicates (std::vector<std::pair<double, double>> &A, std::pair<double, double> base) {
    int size = std::size(A);
    std::vector<std::pair<double, double>> B;
    B.push_back(A[0]);
    for (int i=2; i<size; i++) {
        if (std::abs(compare(base, A[i], A[i-1])) > epsilon) {
            B.push_back(A[i-1]);
        }
    }
    B.push_back(A[size-1]);
    return B;
}


void push_base_point_to_the_front (std::vector<std::pair<double, double>> &A) {
    int base_index = 0;
    int size = std::size(A);
    for (int i=1; i<size; i++) {
        if (A[i].second < A[base_index].second) {
            std::swap(A[i], A[base_index]);
        }
        else if (A[i].second == A[base_index].second) {
            if (A[i].first > A[base_index].second) {
                std::swap(A[i], A[base_index]);
            }
        }
    }
}


void Graham_algorithm (std::vector<std::pair<double, double>>& P, struct ForVis* for_vis, std::shared_ptr<std::vector<std::pair<double, double>>> current_stack) {
    scatter_initial_points(P, for_vis);    
    push_base_point_to_the_front(P);
    std::pair<double, double> base = P[0];
    sort_points_for_Graham(P, base);
    std::vector<std::pair<double, double>> B = remove_deg_duplicates(P, base); 

    int i = 3;
    int index = P.size();
    int current_size = 3;
    int B_size = std::size(B);
    act_accordingly_to_for_vis(for_vis);
    add_line_segment(for_vis, B[0], B[1], &index);
    current_stack->push_back(B[0]);
    current_stack->push_back(B[1]);
    act_accordingly_to_for_vis(for_vis);
    add_line_segment(for_vis, B[1], B[2], &index);
    current_stack->push_back(B[2]);

    while (i < B_size) {
        if (compare(current_stack->at(current_size-2), current_stack->at(current_size-1), B[i]) > epsilon) {
            act_accordingly_to_for_vis(for_vis);
            add_line_segment(for_vis, current_stack->at(current_size-1), B[i], &index);
            current_stack->push_back(B[i]);
            i += 1;
            current_size += 1;
        }
        else {
            act_accordingly_to_for_vis(for_vis);
            remove_last_line_segment(for_vis, &index);
            current_stack->pop_back();
            current_size -= 1;
        }
    }

    act_accordingly_to_for_vis(for_vis);
    add_line_segment(for_vis, current_stack->at(0), current_stack->at(current_size-1), &index);

}