#include "executor.h"
#include "spectrum_analyzer.h"

int main(void)
{
    Executor exec = Executor();
    
    Spec specAnalyzer = Spec();
    specAnalyzer.calcFFT();

    while (1) {
	
    }
}
