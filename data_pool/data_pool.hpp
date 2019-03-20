#ifndef __DATA_POOL_HPP__
#define __DATA_POOL_HPP__
#include <iostream>
#include <string>
#include <vector>
#include <semaphore.h>

class data_pool
{
public:
    data_pool(int _cap = 256);
    void GetData(std::string& outString);   //从数据池中取数据
    void PutData(const std::string& inString);  //向数据池中放数据
    ~data_pool();
private:
    sem_t BlankSem; //空格的资源，表示生产者可用资源
    sem_t DataSem;  //数据的资源，表示消费者可用资源
    std::vector<std::string> pool;  //用vector维护一个环形队列，存放数据（数据池）
    int BlankStep;  //表示生产者的步数
    int DataStep;   //表示消费者的步数
    int cap;        //可存放数据的总容量
};

data_pool::data_pool(int _cap)
    :pool(_cap), BlankStep(0), DataStep(0), cap(_cap)
{
    //初始化信号量
    sem_init(&BlankSem, 0, _cap);
    sem_init(&DataSem, 0, _cap);
}

void data_pool::GetData(std::string& outString)
{
    sem_wait(&BlankSem);
    outString = pool[BlankStep]; //这里可以赋值是因为string中重载过
    ++BlankStep;                 //拿走一个数据以后，空格多了一个
    if (BlankStep >= 256)
    {
        BlankStep = 0;
    }

    sem_post(&DataSem);
}

void data_pool::PutData(const std::string& inString)
{
    sem_wait(&DataSem);
    pool[DataStep] = inString;
    ++DataStep;
    if (DataStep >= 256)
    {
        DataStep = 0;
    }
    sem_post(&BlankSem);
}

data_pool::~data_pool()
{
    sem_destroy(&BlankSem);
    sem_destroy(&DataSem);
}
#endif
