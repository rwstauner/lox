#ifndef CLOX_TABLE_H
#define CLOX_TABLE_H

#include "common.h"
#include "value.h"

typedef struct entry_t {
  object_string_t *key;
  value_t value;
} entry_t;

typedef struct table_t {
  array_meta_t array_meta;
  entry_t *entries;
} table_t;

void table_init(table_t *table);
void table_free(table_t *table);
bool table_get(table_t *table, object_string_t *key, value_t *value);
bool table_set(table_t *table, object_string_t *key, value_t value);
void table_copy_entries(table_t *from, table_t *to);
object_string_t *table_find_string(table_t *from, const char * chars, int length, uint32_t hash);

#endif
