#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fstream>
#include <atomic>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>

// Multithreading
#include <pthread.h>
using namespace std;

// Constants
#define THREAD_POOL 5
int ITERATIONS = 1;
// To write
ofstream file;

class Monitor{
    private:
        int MAX_RESOURCES = 100;
        int RESOURCES_PER_THREAD = 30;
        pthread_mutex_t  MonitorLock;

    public:
    
        void counterInit()
        {
            pthread_mutex_init(&MonitorLock, NULL);
        }

    
        void counterEnd()
        {
            pthread_mutex_destroy(&MonitorLock);
        }

        void decrease()
        {
            printf("entraaaa\n");
            file << "Iniciando decrease_count\n";
            file << "Mutex adquirido, entrando al monitor\n";

            if (MAX_RESOURCES >= RESOURCES_PER_THREAD) {
                file << "Recusos suficientes disponibles, consumiendo... \n";
                pthread_mutex_lock(&MonitorLock);
                for (int i = 0; i < RESOURCES_PER_THREAD; i++) {
                    MAX_RESOURCES--;
                }
                pthread_mutex_unlock(&MonitorLock);
            } else {
                file << "NUAY! Recursos actuales: " << MAX_RESOURCES << "\n";
            }
            file << "Terminando decrease_count\n" ;
        }
};

Monitor monitor;

void* threadWork(void* args) {
    int threadID = syscall(SYS_gettid);
    file << threadID << " - Iniciando iteracion " << ITERATIONS << "\n";
}

int main() {
    file.open("./BitacoraSemaforo2.txt", ofstream::out);
    atomic<int> aux (0); 
    file << "--- Iniciando el programa ---" << "\n";
    pthread_t threadPool[THREAD_POOL];
    monitor.counterInit();

    int mainThreadID = syscall(SYS_gettid);
    file << "Creando Threads" << "\n";
    for (int i = 0; i < THREAD_POOL; i++) {
        file << "Iniciando Threads " << syscall(SYS_gettid) << "\n";
        pthread_create(&threadPool[i], NULL, threadWork, &file);
        monitor.decrease();
        ITERATIONS++;
    }

    for (int i = 0; i < THREAD_POOL; i++) {
        pthread_join(threadPool[i], NULL);
    }

    monitor.counterEnd();
    file << "--- Terminando programa ---" << "\n";
    file.close();
    return 0;
}