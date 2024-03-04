#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "spectrum_analyzer.h"

class Executor {
private:
    Spec specAnalyzer;
    void inline clock_setup(void);
public:
    Executor(void);
    void init(void);
    void exec(void);
};

#endif
