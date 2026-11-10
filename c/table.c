#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

// entry->key should already be NULL before checking this
#define IS_EMPTY_SLOT(entry) (IS_NIL(entry->value))

static entry_t *find_entry(entry_t *entries, int capacity, object_string_t *key) {
  uint32_t index = key->hash % capacity;
  entry_t *tombstone = NULL;

  for (;;) {
    entry_t *entry = &entries[index];
    // If matching key is found or we have an empty slot.
    if (entry->key == NULL) {
      // If value is nil this is not the tombstone entry.
      if (IS_EMPTY_SLOT(entry)) {
        // Return the earliest slot in the probe sequence if there was a collision.
        return tombstone != NULL ? tombstone : entry;
      }
      else {
        // Store the earliest tombstone for later.
        if (tombstone == NULL) tombstone = entry;
      }
    }
    // NOTE: This only works while all strings are interned.
    else if (entry->key == key) {
      return entry;
    }

    // Keep looking for the next empty slot.
    // We will always find one because we resize the table before it ever gets full.
    index = (index + 1) % capacity;
  }
}

static void adjust_capacity(table_t *table, int old_capacity) {
  // capacity has already been updated.
  int capacity = table->array_meta.capacity;
  entry_t *old_entries = table->entries;
  entry_t *new_entries = ALLOCATE(entry_t, capacity);

  for (int i = 0; i < capacity; i++) {
    new_entries[i].key = NULL;
    new_entries[i].value = NIL_VALUE;
  }

  table->array_meta.count = 0;
  for (int i = 0; i < old_capacity; i++) {
    entry_t *entry = &old_entries[i];
    if (entry->key == NULL) continue;

    entry_t *dest = find_entry(new_entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->array_meta.count++;
  }

  if (old_capacity > 0) FREE_ARRAY(entry_t, table->entries, old_capacity);
  table->entries = new_entries;
}

void table_init(table_t *table) {
  table->array_meta.count = 0;
  table->array_meta.capacity = 0;
  table->entries = NULL;
}

void table_free(table_t *table) {
  ARRAY_FREE(&table->array_meta, table->entries);
  table_init(table);
}

bool table_get(table_t *table, object_string_t *key, value_t *value) {
  if (table->array_meta.count == 0) return false;

  entry_t *entry = find_entry(table->entries, table->array_meta.capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

bool table_set(table_t *table, object_string_t *key, value_t value) {
  if (table->array_meta.count + 1 > table->array_meta.capacity * TABLE_MAX_LOAD) {
    int old_capacity = ARRAY_GROW_CAPACITY(&table->array_meta, 1);
    adjust_capacity(table, old_capacity);
  }

  entry_t *entry = find_entry(table->entries, table->array_meta.capacity, key);
  bool is_new = (entry->key == NULL);

  if (is_new && IS_EMPTY_SLOT(entry)) table->array_meta.count++;

  entry->key = key;
  entry->value = value;
  return is_new;
}

bool table_delete(table_t *table, object_string_t *key) {
  if (table->array_meta.count == 0) return false;

  entry_t *entry = find_entry(table->entries, table->array_meta.capacity, key);
  if (entry->key == NULL) return false;

  entry->key = NULL;
  entry->value = BOOL_VALUE(true);
  return true;
}

void table_copy_entries(table_t *from, table_t *to) {
  for (int i = 0; i < from->array_meta.capacity; i++) {
    entry_t *entry = &from->entries[i];
    if (entry->key != NULL) {
      table_set(to, entry->key, entry->value);
    }
  }
}

object_string_t *table_find_string(table_t *table, const char *chars, int length, uint32_t hash) {
  if (table->array_meta.count == 0) return NULL;

  uint32_t index = hash % table->array_meta.capacity;

  for (;;) {
    entry_t *entry = &table->entries[index];
    if (entry->key == NULL) {
      // Stop if we find an empty non-tombstone slot.
      if (IS_EMPTY_SLOT(entry)) return NULL;
    }
    else if (entry->key->length == length &&
             entry->key->hash == hash &&
             memcmp(entry->key->chars, chars, length) == 0) {
      return entry->key;
    }

    index = (index + 1) % table->array_meta.capacity;
  }
}
