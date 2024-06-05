#include "qcustomplot.h"

#ifndef FOR_VIS_H
#define FOR_VIS_H


struct ForVis {
    QCustomPlot* plot_ptr_;
    int sleep_time_;
    bool should_halt_;
};


#endif