#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log_processor.h"

// 1. Prepara la estructura para empezar a contar desde cero
void init_stats(LogStats* stats) {
    memset(stats, 0, sizeof(LogStats));
}

// 2. Convierte un texto en un número de posición
unsigned int hash_function(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// 3. Guarda el dato en la tabla o le suma 1 si ya existe
void insert_or_update(HashNode** table, const char* key, int increment, int* unique_counter) {
    unsigned int index = hash_function(key);
    HashNode* current = table[index];

    // Busca si la IP o URL ya está en la tabla
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->count += increment; // Si existe, solo le suma al contador
            return;
        }
        current = current->next;
    }

    // Si no existía, crea una nueva "caja" (HashNode)
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    strcpy(new_node->key, key);
    new_node->count = increment;
    new_node->next = table[index];
    table[index] = new_node;

    // Si nos pidieron contar los únicos (como en las IPs), le suma 1 al total
    if (unique_counter != NULL) {
        (*unique_counter)++;
    }
}

// 4. Lee una línea de texto del archivo y extrae lo importante
void parse_log_line(char* line, LogStats* stats) {
    char ip[MAX_IP_LEN];
    char url[MAX_URL_LEN];
    int status;

    // Corta la línea y saca la IP, la URL y el código HTTP
    if (sscanf(line, "%19s - - [%*[^]]] \"%*s %255[^\"]\" %d", ip, url, &status) == 3) {

        insert_or_update(stats->ip_table, ip, 1, &stats->unique_ips);
        insert_or_update(stats->url_table, url, 1, NULL);

        // Suma 1 si el código HTTP es un error (400 al 599)
        if (status >= 400 && status <= 599) {
            stats->total_errors++;
        }
    }
}

// 5. Junta los resultados de un hilo con los resultados totales
void merge_stats(LogStats* global, LogStats* local) {
    global->total_errors += local->total_errors;

    for (int i = 0; i < HASH_SIZE; i++) {
        // Pasa las IPs del hilo local al total global
        HashNode* current_ip = local->ip_table[i];
        while (current_ip != NULL) {
            insert_or_update(global->ip_table, current_ip->key, current_ip->count, &global->unique_ips);
            current_ip = current_ip->next;
        }

        // Pasa las URLs del hilo local al total global
        HashNode* current_url = local->url_table[i];
        while (current_url != NULL) {
            insert_or_update(global->url_table, current_url->key, current_url->count, NULL);
            current_url = current_url->next;
        }
    }
}

// 6. Destruye las tablas para devolverle la memoria a la computadora
void free_stats(LogStats* stats) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* current = stats->ip_table[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }

        current = stats->url_table[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
}