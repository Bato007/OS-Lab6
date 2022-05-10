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

        void* decrease();
};

void* Monitor::decrease() {
    printf("entraaaa\n");
    pthread_mutex_lock(&MonitorLock);
    file << "Iniciando Threads " << syscall(SYS_gettid) << "\n";
    
    file << "Iniciando decrease_count\n";
    file << "Mutex adquirido, entrando al monitor\n";

    if (MAX_RESOURCES >= RESOURCES_PER_THREAD) {
        file << "Recusos suficientes disponibles, consumiendo... \n";
        for (int i = 0; i < RESOURCES_PER_THREAD; i++) {
            MAX_RESOURCES--;
        }
    } else {
        file << "NUAY! Recursos actuales: " << MAX_RESOURCES << "\n";
    }
    file << "Terminando decrease_count\n" ;
    pthread_mutex_unlock(&MonitorLock);
}

typedef void* (*THREADFUNCMONITOR) (void *);

int main() {
    Monitor* monitor = new Monitor();
    file.open("./BitacoraSemaforo2.txt", ofstream::out);
    file << "--- Iniciando el programa ---" << "\n";
    pthread_t threadPool[THREAD_POOL];

    file << "Creando Threads" << "\n";
    for (int i = 0; i < THREAD_POOL; i++) {
        pthread_create(&threadPool[i], NULL, (THREADFUNCMONITOR)&Monitor::decrease, monitor);
        ITERATIONS++;
    }

    for (int i = 0; i < THREAD_POOL; i++) {
        pthread_join(threadPool[i], NULL);
    }

    file << "--- Terminando programa ---" << "\n";
    file.close();
    return 0;
}