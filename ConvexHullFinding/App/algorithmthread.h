#include <QThread>
#include <QDebug>
#include <utility>
#include <functional>
#include <vector>
#include <utility>
#include <QMetaObject>
#include "pointslog.h"
#include <QTimer>
#include <QVariant>

#ifndef ALGORITHM_THREAD_H
#define ALGORITHM_THREAD_H


namespace Ui {
    class AlgorithmThread;
}


class AlgorithmThread : public QThread {

    Q_OBJECT

public:

    AlgorithmThread(QObject* parent) : 
        QThread(parent) {;}

// signals:

//     void new_point_to_add_emit (double a, double b);

//     void remove_certain_element_emit (double a, double b);

public:    

    void set_lambda (std::function<void()> lambda_function) {
        this->lambda_function_ = lambda_function;
    }

protected:

    void run() override {
        lambda_function_();
    }

private:

    std::function<void()> lambda_function_;

};


#endif