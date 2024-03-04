#ifndef EXECUTOR_H
#define EXECUTOR_H

class Executor {
private:
    void inline clock_setup(void);
public:
    Executor(void);
    void init(void);
    bool exec(void);
};

#endif
