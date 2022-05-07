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
#include <semaphore.h>

using namespace std;

// Semaphore
sem_t semaphore;

// Constants
#define THREAD_POOL 5
#define ITERATIONS  5
#define RESOURCES   1

// To write
ofstream file;

void* threadWork(void* args) {
    int threadID = syscall(SYS_gettid);
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&semaphore);   // Wait if needed
        file << threadID << " - Iniciando iteracion " << i << "\n";
        file << threadID << " - (!) Recurso tomado" << "\n";

        int wait = rand() % 5 + 1;
        sleep(wait);
        file << threadID << " - ( ) Recurso usado" << "\n";

        sem_post(&semaphore);   // Free source
        file << threadID << " - (X) Recurso devuelto" << "\n";
    }
    file << threadID << " - (-) Thread terminado" << "\n";
}

int main() {
    file.open("./BitacoraSemaforo.txt", ofstream::out);
    atomic<int> aux (0); 
    file << "--- Iniciando el programa ---" << "\n";
    pthread_t threadPool[THREAD_POOL];
    sem_init(&semaphore, 0, 1);
    file << "Se inicio el semaforo" << "\n";
    srand(time(NULL));

    int mainThreadID = syscall(SYS_gettid);
    file << "Creando Threads" << "\n";
    for (int i = 0; i < THREAD_POOL; i++) {
        file << "Iniciando Threads " << syscall(SYS_gettid) << "\n";
        pthread_create(&threadPool[i], NULL, threadWork, &file);
    }

    for (int i = 0; i < THREAD_POOL; i++) {
        aux += 1;
        if (aux == 1) {
            file << "Main esperando a threads" << "\n";
        }
        pthread_join(threadPool[i], NULL);
    }
    file << "--- Terminando programa ---" << "\n";
    file.close();
    return 0;
}