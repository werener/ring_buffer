#include "ring_buffer.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_NOT_NULL_OR(p, ...)                                             \
    do {                                                                       \
        if ((p) == NULL) {                                                     \
            __VA_ARGS__                                                        \
        }                                                                      \
    } while (0)

// Helpers

// Wrapping increment
static inline size_t wrap_inc(size_t idx, size_t cap) {
    return (idx + 1 == cap) ? 0 : idx + 1;
}

// Wrapping decrement
static inline size_t wrap_dec(size_t idx, size_t cap) {
    return (idx == 0) ? cap - 1 : idx - 1;
}
// Indexing into buffer
static inline void *rbuf_get_slot_at(const rbuf_t *rbuf, size_t idx) {
    return (uint8_t *)rbuf->buffer + (idx * rbuf->item_size);
}

const size_t MAX_ALLOWED_SIZE_BYTES = 1024 * 1024 * 4; // 4 MB

rbuf_t rbuf_create(size_t capacity, size_t item_size) {
    if (item_size == 0 || capacity == 0)
        return (rbuf_t){0};
    if (capacity > MAX_ALLOWED_SIZE_BYTES / item_size)
        capacity = MAX_ALLOWED_SIZE_BYTES / item_size;

    rbuf_t rbuf = {
        .buffer = malloc(capacity * item_size),
        .capacity = capacity,
        .count = 0,
        .item_size = item_size,
        .tail = 0,
        .head = 0,
    };
    ASSERT_NOT_NULL_OR(rbuf.buffer, return (rbuf_t){0};);

    return rbuf;
}

rbuf_t *rbuf_allocate(size_t capacity, size_t item_size) {
    rbuf_t stack_rbuf = rbuf_create(capacity, item_size);
    ASSERT_NOT_NULL_OR(stack_rbuf.buffer, return NULL;);

    rbuf_t *rbuf = malloc(sizeof(rbuf_t));
    ASSERT_NOT_NULL_OR(rbuf, {
        rbuf_destroy(&stack_rbuf);
        return NULL;
    });

    *rbuf = stack_rbuf;
    return rbuf;
}

void rbuf_destroy(rbuf_t *rbuf) {
    ASSERT_NOT_NULL_OR(rbuf, return;);

    free(rbuf->buffer);
    *rbuf = (rbuf_t){0};
}

void rbuf_free(rbuf_t *rbuf) {
    ASSERT_NOT_NULL_OR(rbuf, return;);
    rbuf_destroy(rbuf);
    free(rbuf);
}

bool rbuf_is_empty(const rbuf_t *rbuf) { return rbuf->count == 0; }

const void *rbuf_peek_tail(const rbuf_t *rbuf) {
    if (rbuf_is_empty(rbuf))
        return NULL;
    return rbuf_get_slot_at(rbuf, rbuf->tail);
}

const void *rbuf_peek_head(const rbuf_t *rbuf) {
    if (rbuf_is_empty(rbuf))
        return NULL;
    return rbuf_get_slot_at(rbuf, rbuf->head);
}

bool rbuf_copy_tail(const rbuf_t *rbuf, void *restrict dest) {
    const void *src = rbuf_peek_tail(rbuf);
    ASSERT_NOT_NULL_OR(src, return false;);

    (void)memcpy(dest, src, rbuf->item_size);

    return true;
}

bool rbuf_copy_head(const rbuf_t *rbuf, void *restrict dest) {
    const void *src = rbuf_peek_head(rbuf);
    ASSERT_NOT_NULL_OR(src, return false;);

    (void)memcpy(dest, src, rbuf->item_size);

    return true;
}

void rbuf_push_tail(rbuf_t *rbuf, const void *restrict element) {
    if (rbuf_is_empty(rbuf)) {
        // First item lands at index 0; both tail and head point to it
        rbuf->tail = 0;
        rbuf->head = 0;
        rbuf->count = 1;
    } else {
        rbuf->tail = wrap_inc(rbuf->tail, rbuf->capacity);

        if (rbuf->count < rbuf->capacity)
            rbuf->count++;
        else
            // Overwrote the oldest element, so advance head
            rbuf->head = wrap_inc(rbuf->head, rbuf->capacity);
    }

    (void)memcpy(rbuf_get_slot_at(rbuf, rbuf->tail), element, rbuf->item_size);
}

void rbuf_push_head(rbuf_t *rbuf, const void *restrict element) {
    if (rbuf_is_empty(rbuf)) {
        // First item lands at index 0; both head and tail point to it
        rbuf->head = 0;
        rbuf->tail = 0;
        rbuf->count = 1;
    } else {
        rbuf->head = wrap_dec(rbuf->head, rbuf->capacity);

        if (rbuf->count < rbuf->capacity)
            rbuf->count++;
        else
            // Overwrote the element at tail, so retreat tail
            rbuf->tail = wrap_dec(rbuf->tail, rbuf->capacity);
    }

    (void)memcpy(rbuf_get_slot_at(rbuf, rbuf->head), element, rbuf->item_size);
}

bool rbuf_pop_tail(rbuf_t *rbuf) {
    if (rbuf_is_empty(rbuf)) {
        return false;
    }

    rbuf->count--;
    if (rbuf->count == 0) {
        rbuf->head = 0;
        rbuf->tail = 0;
    } else {
        rbuf->tail = wrap_dec(rbuf->tail, rbuf->capacity);
    }

    return true;
}

bool rbuf_pop_head(rbuf_t *rbuf) {
    if (rbuf_is_empty(rbuf)) {
        return false;
    }

    rbuf->count--;
    if (rbuf->count == 0) {
        rbuf->head = 0;
        rbuf->tail = 0;
    } else {
        rbuf->head = wrap_inc(rbuf->head, rbuf->capacity);
    }

    return true;
}