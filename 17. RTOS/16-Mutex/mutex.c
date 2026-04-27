#include "bsw.h"
#include "mutex.h"

void InitMutex(MutexType *mutex, EventMaskType event)
{
  mutex->flag = UNLOCKED;
  mutex->waiting_task = -1;
  mutex->event = event;
  mutex->has_waiter = 0;
}

void GetMutex(MutexType *mutex)
{
  if (mutex->flag == LOCKED) {
    printOut("  --> BLock");
    mutex->has_waiter = 1;
    GetTaskID(&(mutex->waiting_task));
    WaitEvent(mutex->event);
    ClearEvent(mutex->event);
  }
  mutex->flag = LOCKED;
}

void ReleaseMutex(MutexType *mutex)
{
  if (mutex->flag == LOCKED) {
    mutex->flag = UNLOCKED;
    if (mutex->has_waiter) {
      SetEvent(mutex->waiting_task, mutex->event);
      mutex->has_waiter = 0;
    }
  }
}
