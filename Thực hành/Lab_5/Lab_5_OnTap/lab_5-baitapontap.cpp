#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Khai báo các semaphore và các biến global
sem_t sem_1, sem_2, sem_3, sem_4, sem_5, sem_6, sem_7, sem_8;
int x1 = 1;
int x2 = 2;
int x3 = 3;
int x4 = 4;
int x5 = 5;
int x6 = 6;
int w, v, z, y, x;
int ans = 0;

// Hàm thực thi tính toán cho process_1
void *process_1(void *arg)
{
    w = x1 * x2;
    printf("w = %d\n", w);
    sem_post(&sem_1); // tính hiệu semaphore để cho biết w đã được tính báo cho process 5
    sem_post(&sem_2); // tính hiệu semaphore để cho biết w đã được tính báo cho process 6
    sleep(1);
    return NULL;
}

// Tương tự như process_1
void *process_2(void *arg)
{
    v = x3 * x4;
    printf("v = %d\n", v);
    sem_post(&sem_3); // tính hiệu semaphore để cho biết v đã được tính, báo cho process 3
    sem_post(&sem_4); // tính hiệu semaphore để cho biết v đã được tính, báo cho process 4
    sleep(1);
    return NULL;
}

// Tương tự như process_1
void *process_3(void *arg)
{
    sem_wait(&sem_3); // Chờ semaphore 3 được kích hoạt, tức là v đã được tính
    y = v * x5;
    printf("y = %d\n", y);
    sem_post(&sem_5); // Kích hoạt semaphore để cho biết y đã được tính, báo cho process 5
    sleep(1);
    return NULL;
}

// Tương tự như process_1
void *process_4(void *arg)
{
    sem_wait(&sem_4); // Chờ semaphore 4 được kích hoạt, tức là v đã được tính
    z = v * x6;
    printf("z = %d\n", z);
    sem_post(&sem_6); // Kích hoạt semaphore để cho biết z đã được tính, báo cho process 6
    sleep(1);
    return NULL;
}

// Tương tự như process_1
void *process_5(void *arg)
{
    sem_wait(&sem_1); // Chờ semaphore 1 được kích hoạt, tức là w đã được tính
    sem_wait(&sem_5); // Chờ semaphore 5 được kích hoạt, tức là y đã được tính
    y = w * y;
    printf("y = %d\n", y);
    sem_post(&sem_7); // Kích hoạt semaphore để cho biết y đã được tính, báo cho process 7
    sleep(1);
    return NULL;
}

// Tương tự như process_1
void *process_6(void *arg)
{
    sem_wait(&sem_2); // Chờ semaphore 2 được kích hoạt, tức là w đã được tính
    sem_wait(&sem_6); // Chờ semaphore 6 được kích hoạt, tức là z đã được tính
    z = w * z;
    printf("z = %d\n", z);
    sem_post(&sem_8); // Kích hoạt semaphore để cho biết z đã được tính, báo cho process 7
    sleep(1);
    return NULL;
}

// Tương tự như process_1
void *process_7(void *arg)
{
    sem_wait(&sem_7); // Chờ semaphore 7 được kích hoạt, tức là y đã được tính
    sem_wait(&sem_8); // Chờ semaphore 8 được kích hoạt, tức là z đã được tính
    ans = y + z;
    printf("ans = %d\n", ans);
    sleep(1);
    return NULL;
}

int main()
{
    // Khởi tạo semaphore
    sem_init(&sem_1, 0, 0);
    sem_init(&sem_2, 0, 0);
    sem_init(&sem_3, 0, 0);
    sem_init(&sem_4, 0, 0);
    sem_init(&sem_5, 0, 0);
    sem_init(&sem_6, 0, 0);
    sem_init(&sem_7, 0, 0);
    sem_init(&sem_8, 0, 0);

    // Tạo các luồng
    pthread_t th1, th2, th3, th4, th5, th6, th7;
    pthread_create(&th1, NULL, process_1, NULL);
    pthread_create(&th2, NULL, process_2, NULL);
    pthread_create(&th3, NULL, process_3, NULL);
    pthread_create(&th4, NULL, process_4, NULL);
    pthread_create(&th5, NULL, process_5, NULL);
    pthread_create(&th6, NULL, process_6, NULL);
    pthread_create(&th7, NULL, process_7, NULL);

    // Đợi các thread hoàn thành
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);
    pthread_join(th4, NULL);
    pthread_join(th5, NULL);
    pthread_join(th6, NULL);
    pthread_join(th7, NULL);

    return 0;
}

/*
Đoạn mã trên là một chương trình đa luồng sử dụng các semaphore để đồng bộ hóa các phép tính. Dưới đây là mô tả chi tiết về cách chương trình hoạt động:

1. **Khởi tạo Semaphore:** Trong hàm `main()`, các semaphore được khởi tạo với giá trị ban đầu là 0. 
Semaphore được sử dụng để đồng bộ hóa các phép tính và đảm bảo rằng chúng được thực hiện theo đúng thứ tự.

2. **Tạo các luồng:** Trong hàm `main()`, bảy luồng (thread) được tạo bằng cách gọi hàm
 `pthread_create`. Mỗi luồng sẽ thực hiện một phép tính nhất định. Cụ thể là:
   - `process_1` tính toán giá trị của biến `w`.
   - `process_2` tính toán giá trị của biến `v`.
   - `process_3` tính toán giá trị của biến `y`.
   - `process_4` tính toán giá trị của biến `z`.
   - `process_5` tính toán giá trị của biến `y` một lần nữa, sau khi đã có giá trị của `w`.
   - `process_6` tính toán giá trị của biến `z` một lần nữa, sau khi đã có giá trị của `w`.
   - `process_7` tính toán giá trị của biến `ans`, tức là tổng của `y` và `z`.

3. **Đồng bộ hóa các phép tính:** Các semaphore được sử dụng để đảm bảo 
rằng các phép tính được thực hiện theo đúng thứ tự:
   - `sem_1` và `sem_2` đảm bảo rằng `process_1` có thể bắt đầu sau khi `x1 * x2` đã được tính toán và ghi vào `w`.
   - `sem_3` và `sem_4` đảm bảo rằng `process_2` có thể bắt đầu sau khi `x3 * x4` đã được tính toán và ghi vào `v`.
   - `sem_5` đảm bảo rằng `process_3` có thể bắt đầu sau khi `y` đã được tính toán.
   - `sem_6` đảm bảo rằng `process_4` có thể bắt đầu sau khi `z` đã được tính toán.
   - `sem_7` và `sem_8` đảm bảo rằng cả `process_5` và `process_6` đều bắt đầu sau khi `y` và `z` đã được tính toán.

4. **In kết quả:** Khi tất cả các luồng đã hoàn thành, hàm `main()` in ra giá trị của biến `ans`, tức là kết quả của phép tính `y + z`.



*/
