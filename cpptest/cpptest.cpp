#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;

class CalcBlock
{
protected:
    double result = 0;
    virtual double Calc(int iteration, unsigned long totalIterations) = 0;
public:
    void ProcessRange(int begin, int end, int totalIterations)
    {
        for (int i = begin; i < end; i++)
            result += Calc(i, totalIterations);
    }
    double GetResult() const { return result; }
};

class PiCalcBlock : public CalcBlock
{
protected:
    double Calc(int iteration, unsigned long totalIterations) override
    {
        double x_i = (iteration + 0.5) / totalIterations;
        return (double)4 / (1 + (x_i * x_i));
    }
};

int main() {
    unsigned long blockSize = 40;
    unsigned long totalIterations = 100000000;
    int threadCount = 0;

    std::cout << "Input the number of threads:" << endl;
    cin >> threadCount;

    CalcBlock** blocks = new CalcBlock * [threadCount];
    for (int i = 0; i < threadCount; i++)
    {
        blocks[i] = new PiCalcBlock();
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for num_threads(threadCount)
    for (int i = 0; i < totalIterations; i += blockSize)
    {
        unsigned long leftIterations = totalIterations - i, curBlock = blockSize;
        if (leftIterations < blockSize)
            curBlock = leftIterations;
        blocks[omp_get_thread_num()]->ProcessRange(i, i + curBlock, totalIterations);
    }

    double result = 0;
    for (int i = 0; i < threadCount; i++)
        result += blocks[i]->GetResult();
    auto endTime = std::chrono::high_resolution_clock::now();

    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Execution time: " << msec.count() << "ms" << endl;
    std::cout.precision(12);
    std::cout << "result: " << result / totalIterations << endl;

    for (int i = 0; i < threadCount; i++)
        delete blocks[i];
    delete[] blocks;
    return 0;
}
