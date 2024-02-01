#ifndef EXECUTOR_H
#define EXECUTOR_H

class Executor {
private:
    void inline clock_setup(void);
public:
    void init(void);
    bool exec(void);
};

#endif
