#ifdef LOG_PROCESSOR_H
#define LOG_PROCESSOR_H

#define MAX_IP_LEN 16
#define MAX_URL_LEN 260
#define HASH_SIZE 10007

typedef struct HashNode {
    char key[MAX_URL_LEN];
    int count;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* ip_table[HASH_SIZE];
    HashNode* url_table[HASH_SIZE];
    int total_errors;
    int unique_ips;
} LogStats;

void init_stats(LogStats* stats);
void parse_log_line(char* line, LogStats* stats);
void merge_stats(LogStats* global, LogStats* local);
void free_stats(LogStats* stats);

#endif