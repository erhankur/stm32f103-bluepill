#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct _task_t {
    int idx;
    void (*fnc)(void);
    struct _task_t *prev;
    struct _task_t *next;
} task_t;

task_t *_pTask = NULL;
int _nTask = 0;

void Task_Add(void (*fx)(void))
{
    task_t *p;
    
    if (_pTask == NULL) {
        _pTask = malloc(sizeof(task_t));
        _pTask->fnc = fx;
        _pTask->prev = _pTask;
        _pTask->next = _pTask;
        _pTask->idx = _nTask++;
    }
    else {
        p = _pTask->prev;
        
        _pTask->prev = malloc(sizeof(task_t));
        _pTask->prev->fnc = fx;
        _pTask->prev->prev = p;
        _pTask->prev->next = _pTask;
        _pTask->prev->idx = _nTask++;
        
        _pTask->prev->prev->next = _pTask->prev;
    }
}

void Task_Delete(task_t *pTask)
{
    if (pTask != NULL) {
        (pTask->prev)->next = pTask->next;
        (pTask->next)->prev = pTask->prev;
        free(pTask);
        
        pTask = NULL;
        --_nTask;
    }
}

void Task_MoveToHead(int iTask)
{
    int i;
    task_t *p;
    
    if (_pTask == NULL)
        return;
  
    p = _pTask;
    for (i = 0; i < _nTask; ++i) {
        if (p->idx == iTask)
        break;
        p = p->next;
    }
    
    if (i < _nTask) {
        p->prev->next = p->next;
        p->next->prev = p->prev;
        
        p->prev = _pTask;
        p->next = _pTask->next;
        _pTask->next = p;
    }
}

void ISR_F(void)
{
    //...
    Task_MoveToHead(5);
}

void init(void)
{
}

void Task_A(void)
{
    static int i;
    
    ++i;
}

void Task_B(void)
{
    static int i;

    if (i == 1)
        ISR_F();

    ++i;
}

void Task_C(void)
{
    static int i;

    ++i;
}

void Task_D(void)
{
    static int i;

    ++i;
}

void Task_E(void)
{
    static int i;

    ++i;
}

void Task_F(void)
{
    static int i;

    ++i;
}

void Task_G(void)
{
    static int i;

    ++i;
}

void Task_H(void)
{
    static int i;

    ++i;
}

int main_()
{
    init();
  
    while (1) {
        Task_A();
        Task_B();
        Task_C();
        Task_D();
        Task_E();
        Task_F();
        Task_G();
        Task_H();
    }
  
    //return 0;
}

int main()
{
    init();
  
    Task_Add(&Task_A);
    Task_Add(&Task_B);
    Task_Add(&Task_C);
    Task_Add(&Task_D);
    Task_Add(&Task_E);
    Task_Add(&Task_F);
    Task_Add(&Task_G);
    Task_Add(&Task_H);
  
    while (1)
    {
        _pTask->fnc();
        _pTask = _pTask->next;
    }
  
    //return 0;
}
