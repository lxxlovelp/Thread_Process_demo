#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;   // 用来存 fork() 的返回值（区分父子进程）
    int ret;

    // 👇 创建子进程
    pid = fork();

    // ❗ fork 失败
    if (pid == -1) {
        perror("fork");   // 打印错误原因
        return -1;
    }

    // =========================
    // 👶 子进程执行路径
    // =========================
    if (pid == 0) {
        printf("child process running...\n");

        // 🟡 阻塞：等待用户输入（模拟“子进程正在运行”）
        getchar();

        printf("child process normal exit...\n");

        // 🔴 子进程退出，返回状态码 5
        // 👉 注意：5 会被父进程 wait() 捕获
        exit(5);
    }

    // =========================
    // 👨 父进程执行路径
    // =========================
    else {
        int state;   // 用来保存子进程“死亡信息”
        pid_t rc;    // wait() 的返回值（子进程 PID）

        printf("parent process wait...!\n");

        // 🟡 等待子进程结束（阻塞）
        // 👉 一旦子进程 exit，wait 才返回
        rc = wait(&state);

        // ❗ wait 失败
        if (rc == -1) {
            perror("wait");
            exit(0);
        }

        // =========================
        // 确认是不是刚才 fork 的子进程
        // =========================
        if (rc == pid) {

            // 🧾 打印原始状态（十六进制）
            // 👉 state 是“打包后的死亡信息”，不是简单的 5
            printf("catch child process exit state: %x!\n", state);

            // =========================
            // 判断子进程是否“正常退出”
            // =========================
            if (WIFEXITED(state)) {

                // ✅ 正常退出（exit / return）
                // 👉 提取 exit(5) 里的 5
                printf("child process normal exit: %d\n",
                       WEXITSTATUS(state));

            } else {

                // ❌ 非正常退出（比如被 kill -9）
                printf("child normal exit!\n");
            }
        }
    }

    // =========================
    // 父进程结束
    // =========================
    printf("parent process ready exit!\n");

    return 0;
}

// | 函数        | 含义           | 能不能指定等谁 |
// | --------- | ------------ | ------- |
// | wait()    | 随便等一个子进程死   
// | waitpid() | 等指定 PID 的子进程 | 


//                 ┌──────────────────────┐
//                 │        main()        │
//                 └─────────┬────────────┘
//                           │
//                         fork()
//                           │
//           ┌───────────────┴────────────────┐
//           │                                │
//           ▼                                ▼
//  ┌──────────────────┐            ┌──────────────────┐
//  │   子进程 PID=0   │            │   父进程 PID>0   │
//  └──────────────────┘            └──────────────────┘
//           │                                │
//           │                                │
//           ▼                                ▼
// 打印: child running             打印: parent wait...
//           │                                │
//           │                         wait(&state)
//           │                                │
//           ▼                                │
//      getchar()（阻塞）                      │
//           │                                │
//       用户输入                              │
//           │                                │
//           ▼                                │
// 打印: child exit                          │
// exit(5) 结束子进程                         │
//           │                                │
//           └──────────────┐                │
//                          ▼                │
//                  子进程退出               │
//                  (return 5)              │
//                          │                │
//                          └───────┬────────┘
//                                  ▼
//                          wait() 返回父进程
//                                  │
//                          state 被填充
//                                  │
//                                  ▼
//                  ┌──────────────────────────┐
//                  │ 解析 state               │
//                  │                          │
//                  │ WIFEXITED(state) == true │
//                  │ WEXITSTATUS(state) = 5   │
//                  └──────────────────────────┘
//                                  │
//                                  ▼
//                       parent process exit