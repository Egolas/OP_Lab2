#pragma once
#include <unistd.h>
using namespace std;


struct CommonData
{
public:
    pid_t program_id;
    int data;
    CommonData(pid_t pid, int data)
    {
        this->program_id = pid;
        this->data = data;
    }
};