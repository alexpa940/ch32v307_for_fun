#include "FreeRTOS.h"
#include "task.h"

void vApplicationIdleHook(void)
{
    //resetWatchdog();
}

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
   /* ����ݧ� �ҧ��֧��, ���֧է���ѧӧݧ֧ߧߧ��� �էݧ� Idle task, �ҧ��ݧ� �է֧ܧݧѧ�ڧ��ӧѧߧ�
      �ӧߧ���� ����� ���ߧܧ�ڧ�, ��� ��ߧ� �է�ݧاߧ� �ҧ���� �� �ѧ��ڧҧ���� static - �ڧߧѧ��
      ��ߧ� �ҧ�է�� �ӧ��է֧ݧ֧ߧ� �ڧ� ���֧ܧ�, �� ��ߧڧ���اѧ��� ���� �ӧ����է� �ڧ� ����� ���ߧܧ�ڧ�. */
   static StaticTask_t xIdleTaskTCB;
   static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

   *ppxIdleTaskStackBuffer = uxIdleTaskStack;

   /* ���֧�֧էѧ�� �ߧѧ��ا� ��ܧѧ٧ѧ�֧ݧ� �ߧ� �����ܧ���� StaticTask_t, �� �ܧ������
      �٧ѧէѧ�� Idle �ҧ�է֧� �����ѧߧ��� ��ӧ�� �������ߧڧ�. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

   /* ���֧�֧էѧ�� �ߧѧ��ا� ��ѧ٧ާ֧�� �ާѧ��ڧӧ�, �ߧ� �ܧ������� ��ܧѧ٧��ӧѧ֧� *ppxIdleTaskStackBuffer.
      ���ҧ�ѧ�ڧ�� �ӧߧڧާѧߧڧ�, ���� ����ܧ�ݧ�ܧ� �ާѧ��ڧ� �է�ݧا֧� �ҧ���� ��ڧ�� StackType_t, ���
      configMINIMAL_STACK_SIZE ��ܧѧ٧��ӧѧ֧��� �� ��ݧ�ӧѧ�, �� �ߧ� �� �ҧѧۧ�ѧ�. */
   *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* ���ҧ� ��ѧ�ѧާ֧��� configSUPPORT_STATIC_ALLOCATION �� configUSE_TIMERS ����ѧߧ�ӧݧ֧ߧ�
   �� 1, ������ާ� ���ڧݧ�ا֧ߧڧ� �է�ݧاߧ� ���֧է���ѧӧڧ�� ��֧ѧݧڧ٧ѧ�ڧ� vApplicationGetTimerTaskMemory()
   ����ҧ� ��ҧ֧��֧�ڧ�� ��ѧާ���� �٧ѧէѧ�� Timer service. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
   /* ����ݧ� �ҧ��֧��, ���֧է���ѧӧݧ֧ߧߧ��� �էݧ� Timer task, �է֧ܧݧѧ�ڧ��ӧѧߧ� �ӧߧ���� �����
      ���ߧܧ�ڧ�, ��� ��ߧ� �է�ݧاߧ� �ҧ���� �� �ѧ��ڧҧ���� static �C �ڧߧѧ�� �ҧ��֧�� �ҧ�է��
      ��ߧڧ���ا֧ߧ� ���� �ӧ����է� �ڧ� ����� ���ߧܧ�ڧ�. */
   static StaticTask_t xTimerTaskTCB;
   static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

   /* ���֧�֧էѧ�� �ߧѧ��ا� ��ܧѧ٧ѧ�֧ݧ� �ߧ� �����ܧ���� StaticTask_t, �� �ܧ������ �ҧ�է֧�
      �����ѧߧ����� �������ߧڧ� �٧ѧէѧ�� Timer service. */
   *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

   /* ���֧�֧էѧ�� �ߧѧ��ا� �ާѧ��ڧӧ�, �ܧ������� �ҧ�է֧� �ڧ���ݧ�٧�ӧѧ���� �ܧѧ� ���֧�
      �٧ѧէѧ�� Timer service. */
   *ppxTimerTaskStackBuffer = uxTimerTaskStack;

   /* ���֧�֧էѧ�� �ߧѧ��ا� ��ѧ٧ާ֧�� �ާѧ��ڧӧ�, �ߧ� �ܧ������� ��ܧѧ٧��ӧѧ֧� *ppxTimerTaskStackBuffer.
      ���ҧ�ѧ�ڧ�� �ӧߧڧާѧߧڧ�, ���� ����ܧ�ݧ�ܧ� �ާѧ��ڧ� �է�ݧا֧� �ҧ���� ��ڧ�� StackType_t, ���
      configTIMER_TASK_STACK_DEPTH ��ܧѧ٧��ӧѧ֧��� �� ��ݧ�ӧѧ�, �� �ߧ� �� �ҧѧۧ�ѧ�. */
   *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                        char * pcTaskName ) {
  (void)xTask;
  (void)pcTaskName;

  // We dont have a good way to handle a stack overflow at this point in time
  NVIC_SystemReset();
}

void vApplicationMallocFailedHook( void )
{
    NVIC_SystemReset();
}


