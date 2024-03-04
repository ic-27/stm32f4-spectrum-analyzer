#include "executor.h"

int main(void)
{
    Executor exec = Executor();
    
    while (1) {
	exec.exec();
    }
}
