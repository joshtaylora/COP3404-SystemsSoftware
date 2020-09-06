#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define TABLE_SIZE 100001

unsigned int hash(const char *key){
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; i++) {
        value = value * 37 + key[i];
    }

    value = value % TABLE_SIZE;

    return value;
}


//linked list of entries
typedef struct entry_t {
    char *key;
    char *value;
    struct entry_t *next; // pointer to the next entry
} entry_t;

typedef struct {
    entry_t **entries; // array of pointers to the entries
} ht_t;

ht_t *ht_create(void) {
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) *TABLE_SIZE);

    // set each entry to null 
    int i = 0;
    for (; i < TABLE_SIZE; i++) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

entry_t *ht_pair(const char *key, const char *value) {
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);

    // copy the key and value in place
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // next starts out nulll but may be set later on
    entry->next = NULL;

    return entry;
}

void ht_set(ht_t *hashtable, const char *key, const char *value) {
    // generate a hash using the given key
    unsigned int bucket = hash(key);

    entry_t *entry = hashtable->entries[bucket];

    // if the entry is NULL, the slot is empty so we can make a new slot
    if (entry == NULL) {
        hashtable->entries[bucket] = ht_pair(key, value);
        return;
    }

    entry_t *prev;

    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // match found, replace the value
            free(entry->value);
            entry->value = malloc(strlen(value) + 1);
            strcpy(entry->value, value);
            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev-> next = ht_pair(key, value);
}

char *ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);
    
    // try to find a valid slot in the table
    entry_t *entry = hashtable->entries[slot];
    
    // no slot means that the entry is not in the hash table
    if (entry == NULL) {
        return NULL;
    }
    // walk through each entry in the slot, which could just be a single entry
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        // proceed to next key if available
        entry = entry->next;
    }

    // no entry found
    return NULL;
}

// prints what is at every slot of the hashtable
void ht_dump(ht_t *hashtable) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t *entry = hashtable->entries[i];

        if (entry == NULL) { continue; }

        printf("slot[%4d]: ", i);

        for (;;) {
            printf("%s=%s", entry->key, entry->value);

            if (entry->next == NULL) { break; }

            entry = entry->next;
        }

        printf("\n");
    }
}

int main(int argc, char **argv){
    // create the hash table
    ht_t *ht = ht_create();

    // set values in the hashtable
    ht_set(ht, "Josh", "boyfriend");
    ht_set(ht, "Kat", "girlfriend");
    ht_set(ht, "Kerry", "bestfriend");
    ht_set(ht, "Brandon", "bestfriend_boyfriend");
    ht_set(ht, "Kim", "friend");
    ht_set(ht, "Tyler", "friend");
    ht_set(ht, "Elisha", "boyfriends_friend");

    ht_dump(ht);
    
    return 0;
}
