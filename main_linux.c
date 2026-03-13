#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "log_processor.h"
#define MAX_THREADS 4
#define MAX_LINES   100000
#define MAX_LINE_LEN 512
// Datos que recibe cada hilo
typedef struct {
    char**   lines;
    int      start;
    int      end;
    LogStats local_stats;
} ThreadArgs;
// Lo que hace cada hilo: procesar sus líneas
void* process_chunk(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    init_stats(&args->local_stats);

    for (int i = args->start; i < args->end; i++) {
        parse_log_line(args->lines[i], &args->local_stats);
    }

    return NULL;
}
int main() {
    // 1. Abrir el archivo
    FILE* file = fopen("access.log", "r");
    if (!file) {
        printf("Error: no se pudo abrir access.log\n");
        return 1;
    }
    // 2. Leer todas las líneas
    char** lines = malloc(MAX_LINES * sizeof(char*));
    int total = 0;

    while (total < MAX_LINES) {
        lines[total] = malloc(MAX_LINE_LEN);
        if (!fgets(lines[total], MAX_LINE_LEN, file)) {
            free(lines[total]);
            break;
        }
        total++;
    }
    fclose(file);
    // 3. Crear hilos y repartir líneas
    pthread_t  threads[MAX_THREADS];
    ThreadArgs args[MAX_THREADS];
    int chunk = total / MAX_THREADS;

    for (int t = 0; t < MAX_THREADS; t++) {
        args[t].lines = lines;
        args[t].start = t * chunk;
        args[t].end   = (t == MAX_THREADS - 1) ? total : args[t].start + chunk;
        pthread_create(&threads[t], NULL, process_chunk, &args[t]);
    }
    // 4. Esperar a que terminen
    for (int t = 0; t < MAX_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    // 5. Combinar resultados
    LogStats global;
    init_stats(&global);
    for (int t = 0; t < MAX_THREADS; t++) {
        merge_stats(&global, &args[t].local_stats);
        free_stats(&args[t].local_stats);
    }
    // 6. Buscar URL más visitada
    char best_url[MAX_URL_LEN] = "";
    int  best_count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* node = global.url_table[i];
        while (node) {
            if (node->count > best_count) {
                best_count = node->count;
                strcpy(best_url, node->key);
            }
            node = node->next;
        }
    }
    printf("Total Unique IPs: %d\n", global.unique_ips);
    printf("Most Visited URL: %s (%d times)\n", best_url, best_count);
    printf("HTTP Errors: %d\n", global.total_errors);

    // 8. Liberar memoria
    free_stats(&global);
    for (int i = 0; i < total; i++) free(lines[i]);
    free(lines);

    return 0;
}