#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <signal.h>

#define ERROR_CODE 990099
#define PORT 24330
#define MAX_ITEMS 1024
#define MAX_BUF_SIZE 1024
#define MAX_SEMAPHORE 1000
int socket_desc, client_size;
struct sockaddr_in server_addr, client_addr;
void init();
int current_customers_Nr = 0;
int current_products_Nr = 0;
float reducere_flag;
pthread_mutex_t customers_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reducere_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t products_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t semaphore_stock_null;
atomic_int stop_threads = 0;
typedef struct Products
{
    char name[1024];
    int ID;
    float price;
    int nr_bucati;
} Products;

typedef struct Customers
{
    char user_name[1024];
    int ID;
    float amount_of_money;
} Customers;

Products product_list[MAX_ITEMS];
Customers customers_list[MAX_ITEMS];
void insert_into_CustomersList(Customers new_customer);
void insert_into_ProductsList(Products new_product);
void show_listst()
{
    pthread_mutex_lock(&customers_mutex);
    for (int i = 0; i < current_customers_Nr; i++)
    {
        printf("Customer:::%s...ID: %d...%f...\n", customers_list[i].user_name, customers_list[i].ID, customers_list[i].amount_of_money);
    }
    pthread_mutex_unlock(&customers_mutex);

    printf("\n");
    pthread_mutex_lock(&products_mutex);
    for (int i = 0; i < current_products_Nr; i++)
    {
        printf("Product:::%s...ID:%d...%f....%d...\n", product_list[i].name, product_list[i].ID, product_list[i].price, product_list[i].nr_bucati);
    }
    pthread_mutex_unlock(&products_mutex);
}
int get_USER_By_ID(int user_id)
{
    pthread_mutex_lock(&customers_mutex);
    for (int i = 0; i < current_customers_Nr; i++)
    {
        if (customers_list[i].ID == user_id)
        {
            pthread_mutex_unlock(&customers_mutex);

            return i;
        }
    }
    pthread_mutex_unlock(&customers_mutex);

    return ERROR_CODE;
}
int get_PRODUCT_By_ID(int product_id)
{
    pthread_mutex_lock(&products_mutex);
    for (int i = 0; i < current_products_Nr; i++)
    {
        if (product_list[i].ID == product_id)
        {
            pthread_mutex_unlock(&products_mutex);

            return i;
        }
    }
    pthread_mutex_unlock(&products_mutex);

    return ERROR_CODE;
}
int verify_Transaction(int current_customer, int current_product)
{
    pthread_mutex_lock(&customers_mutex);
    pthread_mutex_lock(&products_mutex);
    if (customers_list[current_customer].amount_of_money >= product_list[current_product].price)
    {
        pthread_mutex_unlock(&customers_mutex);
        pthread_mutex_unlock(&products_mutex);

        return 0;
    }
    return 1;
}
void delete_stock()
{
    int index_to_delete = 0;
    pthread_mutex_lock(&products_mutex);
    for (int i = 0; i < current_products_Nr; i++)
    {
        if (product_list[i].nr_bucati == 0)
        {
            index_to_delete = i;
            // delete
        }
    }

    for (int i = index_to_delete; i < current_products_Nr; i++)
    {
        product_list[i] = product_list[i + 1];
    }
    current_products_Nr--;
    pthread_mutex_unlock(&products_mutex);
}
int verify_stock_child_proc()
{
    int sum = 0;
    pthread_mutex_lock(&products_mutex);
    for (int i = 0; i < current_products_Nr; i++)
    {
        sum += product_list[i].nr_bucati;
    }
    pthread_mutex_unlock(&products_mutex);
    return sum;
}
void *handle_every_clientByThread(void *args)
{
    int client_sock = *((int *)args);
    // make the reacv to get de client command:
    // Receive client's message:
    // We now use client_sock, not socket_desc
    show_listst();
    char buffer[MAX_BUF_SIZE];
    if (recv(client_sock, buffer, sizeof(buffer), 0) < 0)
    {
        printf("Couldn't receive\n");
        exit(-1);
    }
    printf("Recv from client: %s....\n\n", buffer);
    char case_child_proc[100];
    char *saver_ptr = NULL;
    char *starter = NULL;
    starter = strtok_r(buffer, " ", &saver_ptr);
    int user_id = atoi(starter);
    strcpy(case_child_proc, starter);
    if (strcmp(case_child_proc, "stock") == 0)
    {
        int sum = verify_stock_child_proc();
        send(client_sock, &sum, sizeof(sum), 0);
        close(client_sock);
        return NULL;
    }
    starter = strtok_r(NULL, " ", &saver_ptr);
    int product_id = atoi(starter);

    int current_customer_index = get_USER_By_ID(user_id);
    if (current_customer_index == ERROR_CODE)
    {
        // wrong user id
        char message[MAX_BUF_SIZE];
        strcpy(message, "WRONG CUSTOMER ID");
        send(client_sock, message, sizeof(message), 0);
        close(client_sock);
        return NULL;
    }
    int current_product_index = get_PRODUCT_By_ID(product_id);
    if (current_product_index == ERROR_CODE)
    {
        // wrong user id
        char message[MAX_BUF_SIZE];
        strcpy(message, "WRONG PRODUCT ID");
        send(client_sock, message, sizeof(message), 0);
        close(client_sock);
        return NULL;
    }
    if (verify_Transaction(current_customer_index, current_product_index) == 0)
    {
        // allow

        pthread_mutex_lock(&customers_mutex);
        pthread_mutex_lock(&products_mutex);
        product_list[current_product_index].nr_bucati--;
        if (product_list[current_product_index].nr_bucati == 0)
        {
            sem_post(&semaphore_stock_null);
        }
        customers_list[current_customer_index].amount_of_money -= product_list[current_product_index].price;
        pthread_mutex_unlock(&customers_mutex);
        pthread_mutex_unlock(&products_mutex);
        char message[MAX_BUF_SIZE];
        strcpy(message, "PURCHASE SUCCESFULL!!!!");
        send(client_sock, message, sizeof(message), 0);
        close(client_sock);
        return NULL;
    }
    else
    {
        char message[MAX_BUF_SIZE];
        strcpy(message, "NOT ENOUGHT MONEY");
        send(client_sock, message, sizeof(message), 0);
        close(client_sock);
        return NULL;
        // deny
    }
    close(client_sock);
    return NULL;
}
void read_files_and_make_the_DB();
void *stock_verifier_handler_singleThreaded(void *args)
{
    while (atomic_load(&stop_threads) == 0)
    {
        sem_wait(&semaphore_stock_null);
        // delete product
        delete_stock();
    }
    return NULL;
}
void *recv_stdin_from_admin(void *args)
{
    char buffer[1024];
    while (atomic_load(&stop_threads) == 0)
    {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            printf("Recv from std_in: %s", buffer);
            char *saver_ptr = NULL;
            char *starter = NULL;
            char field[MAX_BUF_SIZE];

            starter = strtok_r(buffer, " ", &saver_ptr);
            int product_id = atoi(starter);
            starter = strtok_r(NULL, " ", &saver_ptr);
            strcpy(field, starter);
            starter = strtok_r(NULL, " ", &saver_ptr);
            int amound = atoi(starter);
            if (strcmp(field, "cant") == 0)
            {
                pthread_mutex_lock(&products_mutex);
                product_list[product_id].nr_bucati += amound;
                pthread_mutex_unlock(&products_mutex);
            }
            if (strcmp(field, "red") == 0)
            {
                float reducere = 0;
                pthread_mutex_lock(&products_mutex);
                for (int i = 0; i < current_products_Nr; i++)
                {
                    reducere = (amound / 100) * (product_list[i].price);
                    product_list[i].price -= reducere;
                }
                pthread_mutex_unlock(&products_mutex);
                pthread_mutex_lock(&reducere_mutex);
                reducere_flag = reducere;
                pthread_mutex_unlock(&reducere_mutex);
            }
            else
            {
                printf("BAD INPUT FROM ADMIN....TRY AGAIN...!!!!\n");
            }
        }
    }
    return NULL;
}
void signal_handler(int sig)
{
    pthread_mutex_lock(&reducere_mutex);
    if (reducere_flag != 0)
    {
        pthread_mutex_lock(&products_mutex);
        for (int i = 0; i < current_products_Nr; i++)
        {
            product_list[i].price += reducere_flag;
        }
        pthread_mutex_unlock(&products_mutex);
        reducere_flag = 0;
        pthread_mutex_unlock(&reducere_mutex);
    }
    else
    {
        printf("PRIMIRE SEMNAL BUT NO REDUCERE ACTIVE");
    }
}
void init_signal()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGRTMIN, &sa, NULL);
}
void talk_to_parent_from_child_proc();

int main(void)
{
    init();
    init_signal();
    read_files_and_make_the_DB();
    sem_init(&semaphore_stock_null, 0, 0);
    pthread_t verify_stock_pthread;
    if (pthread_create(&verify_stock_pthread, NULL, &stock_verifier_handler_singleThreaded, NULL) < 0)
    {
        perror("stock verify thread create error");
        exit(EXIT_FAILURE);
    }
    pthread_t stdin_pthread;
    if (pthread_create(&stdin_pthread, NULL, &recv_stdin_from_admin, NULL) < 0)
    {
        perror("stock verify thread create error");
        exit(EXIT_FAILURE);
    }
    pthread_t tid[2000];
    int client_vec_sock[2000];
    int client_count = 0;
    pid_t pid = fork();
    switch (pid)
    {
    case 0:
    {
        talk_to_parent_from_child_proc();
        exit(0);
        break;
    }
    case -1:
    {
        perror("error AT FORK");
        exit(EXIT_FAILURE);
    }
    default:
    {
        // parent

        while (1)
        {

            client_size = sizeof(client_addr);
            client_vec_sock[client_count] = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
            if (client_vec_sock[client_count] < 0)
            {
                printf("Can't accept\n");
                return -1;
            }
            printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            pthread_create(&tid[client_count], NULL, &handle_every_clientByThread, (void *)&client_vec_sock[client_count]);
            client_count++;
        }
        pthread_join(verify_stock_pthread, NULL);
        pthread_join(stdin_pthread, NULL);
        for (int i = 0; i < client_count; i++)
        {
            pthread_join(tid[i], NULL);
        }
        close(socket_desc);

        return 0;
    }
    }
}
//
//
//
//
//
//
//
//
//
void talk_to_parent_from_child_proc()
{
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];

    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    strcpy(client_message, "stock");
    int fd = open("log.txt", O_RDWR);
    while (atomic_load(&stop_threads) == 0)
    {
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);

        if (socket_desc < 0)
        {
            printf("Unable to create socket\n");
            exit(1);
        }

        printf("Socket created successfully\n");

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            printf("Unable to connect\n");
            exit(1);
        }
        if (send(socket_desc, client_message, strlen(client_message), 0) < 0)
        {
            printf("Unable to send message\n");
            exit(1);
        }

        if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0)
        {
            printf("Error while receiving server's msg\n");
            exit(1);
        }
        int received_int;
        memcpy(&received_int, server_message, sizeof(int));

        printf("Server's response: %d\n", received_int);
        if (received_int == 0)
        {
            kill(getppid(), SIGRTMIN);
        }
        else
        {
            time_t raw_time;
            struct tm *time_info;
            char buffer_time[80];

            time(&raw_time);
            time_info = localtime(&raw_time);

            strftime(buffer_time, sizeof(buffer_time), "%Y-%m-%d %H:%M:%S", time_info);

            char buffer[1024];
            sprintf(buffer, "amound: %d....time:%s...\n", received_int, buffer_time);
            int byres_written = write(fd, buffer, sizeof(buffer) * sizeof(char));
        }

        sleep(5);
    }
    close(fd);
}

void read_files_and_make_the_DB()
{
    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));

    FILE *file = fopen("customers.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file customers");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        Customers new_customer;

        char *start_line = strtok(buffer, ",");
        new_customer.ID = atoi(start_line);

        start_line = strtok(NULL, ",");
        strcpy(new_customer.user_name, start_line);

        start_line = strtok(NULL, ",");
        new_customer.amount_of_money = atof(start_line);

        insert_into_CustomersList(new_customer);
    }

    fclose(file);

    FILE *fil2 = fopen("products.txt", "r");
    if (fil2 == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    memset(buffer, '\0', sizeof(buffer));
    while (fgets(buffer, sizeof(buffer), fil2) != NULL)
    {
        Products new_product;
        char *start_line = strtok(buffer, ",");
        // id
        new_product.ID = atoi(start_line);
        start_line = strtok(NULL, ",");
        // nume
        strcpy(new_product.name, start_line);
        start_line = strtok(NULL, ",");
        // bani
        new_product.price = atof(start_line);

        start_line = strtok(NULL, ",");
        // bani
        new_product.nr_bucati = atoi(start_line);

        insert_into_ProductsList(new_product);
    }
    fclose(fil2);
}
void insert_into_CustomersList(Customers new_customer)
{
    customers_list[current_customers_Nr] = new_customer;
    current_customers_Nr++;
}
void insert_into_ProductsList(Products new_product)
{
    product_list[current_products_Nr] = new_product;
    current_products_Nr++;
}
void init()
{
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        exit(-1);
    }
    printf("Socket created successfully\n");

    // Set port and IP that we'll be listening for, any other IP_SRC or port will be dropped:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        exit(-1);
    }
    printf("Done with binding\n");

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening\n");
        exit(-1);
    }
    printf("\nListening for incoming connections.....\n");
}