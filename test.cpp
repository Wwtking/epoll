// #include <stdio.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <unistd.h>
// #define MAXNUM (2)
// sem_t semDownload;
// pthread_t a_thread, b_thread, c_thread;
// int g_phreadNum = 1;

// void* func1(void *arg)
// {
//     // 等待信号量的值 > 0
//     sem_wait(&semDownload);
//     printf("============== Downloading taskType 1 ============== \n");
//     sleep(5);
//     printf("============== Finished taskType 1 ============== \n");
//     g_phreadNum--;
//     // 等待线程结束
//     pthread_join(a_thread, NULL);
//     return nullptr;
// }

// void* func2(void *arg)
// {
//     sem_wait(&semDownload);
//     printf("============== Downloading taskType 2 ============== \n");
//     sleep(3);
//     printf("============== Finished taskType 2 ============== \n");
//     g_phreadNum--;
//     pthread_join(b_thread, NULL);
//     return nullptr;
// }

// void* func3(void *arg)
// {
//     sem_wait(&semDownload);
//     printf("============== Downloading taskType 3 ============== \n");
//     sleep(1);
//     printf("============== Finished taskType 3 ============== \n");
//     g_phreadNum--;
//     pthread_join(c_thread, NULL);
//     return nullptr;
// }

// int main()
// {
//     // 初始化信号量
//     sem_init(&semDownload, 0, 0);
//     int taskTypeId;
//     while (scanf("%d", &taskTypeId) != EOF)
//     {
//         // 输入 0, 测试程序是否能正常退出
//         if (taskTypeId == 0 && g_phreadNum <= 1)
//         {
//             break;
//         } 
//         else if (taskTypeId == 0)
//         {
//             printf("Can not quit, current running thread num is %d\n", g_phreadNum - 1);
//         }
//         printf("your choose Downloading taskType %d\n", taskTypeId);
//         // 线程数超过 2 个则不下载
//         if (g_phreadNum > MAXNUM)
//         {
//             printf("!!! You've reached the max number of threads !!!\n");
//             continue;
//         }
//         // 用户选择下载 Task
//         switch (taskTypeId)
//         {
//         case 1:
//             // 创建线程 1
//             pthread_create(&a_thread, NULL, func1, NULL);
//             // 信号量 + 1，进而触发 func1 的任务
//             sem_post(&semDownload);
//             // 总线程数 + 1
//             g_phreadNum++;
//             break;
//         case 2:
//             pthread_create(&b_thread, NULL, func2, NULL);
//             sem_post(&semDownload);
//             g_phreadNum++;
//             break;
//         case 3:
//             pthread_create(&c_thread, NULL, func3, NULL);
//             sem_post(&semDownload);
//             g_phreadNum++;
//             break;
//         default:
//             printf("!!! error taskTypeId %d !!!\n", taskTypeId);
//             break;
//         }
//     }
//     // 销毁信号量
//     sem_destroy(&semDownload);
//     return 0;
// }


// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>

// volatile int count = 0;

// void* thread_func(void* arg)
// {
//     std::cout << "Thread started." << std::endl;
//     // do something
//     for(int i = 0; i < 100000; ++i) {
//         ++count;
//     }
//     std::cout << "Thread finished." << std::endl;
//     return NULL;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, NULL, thread_func, NULL);
//     pthread_detach(tid);   // 将线程标记为“可分离的”
//     std::cout << "Main thread finished." << std::endl;
//     sleep(3);
//     return 0;
// }

// #include <iostream>
// #include <ucontext.h>

// ucontext_t context_one, context_two, context_main;

// void thread_one()
// {
//     std::cout << "Thread One start\n";
//     swapcontext(&context_one, &context_two);
//     std::cout << "Thread One end\n";
//     swapcontext(&context_one, &context_two);
// }

// void thread_two()
// {
//     std::cout << "Thread Two start\n";
//     swapcontext(&context_two, &context_one);
//     std::cout << "Thread Two end\n";
//     swapcontext(&context_two, &context_main);
//     std::cout << "never reach!\n";
// }

// int main()
// {
//     char stack_one[8192];
//     char stack_two[8192];
//     getcontext(&context_one);
//     getcontext(&context_two);
//     getcontext(&context_main);
//     context_one.uc_link = 0;
//     context_one.uc_stack.ss_sp = stack_one;
//     context_one.uc_stack.ss_size = sizeof(stack_one);
//     makecontext(&context_one, (void (*)(void)) thread_one, 0);
//     context_two.uc_link = 0;
//     context_two.uc_stack.ss_sp = stack_two;
//     context_two.uc_stack.ss_size = sizeof(stack_two);
//     makecontext(&context_two, (void (*)(void)) thread_two, 0);
//     std::cout << "Main start\n";
//     swapcontext(&context_main, &context_one);
//     std::cout << "Main end\n";
//     return 0;
// }

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char **argv) {
	DIR *pDir = NULL;
	struct dirent * pEnt = NULL;
	unsigned int cnt = 0;	
	if (argc != 2)
	{
		printf("usage: %s dirname\n", argv[0]);
		return -1;
	}
	pDir = opendir(argv[1]);
	if (NULL == pDir)
	{
		perror("opendir");
		return -1;
 
	}	
	while (1)
	{
		pEnt = readdir(pDir);
		if(pEnt != NULL)
		{
			if (pEnt->d_type == DT_REG)
			{
				printf("是普通文件:");
			}
			else
			{
				printf("不是普通文件:");
			}
			printf("name：[%s]	\n", pEnt->d_name);
			cnt++;
		}
		else
		{
			break;
		}
	};
	printf("总文件数为：%d\n", cnt);
	return 0;
}