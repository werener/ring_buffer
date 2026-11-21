/**
 * @file ring_buffer.h
 * @brief Ring buffer data structure and operations.
 */

/** @brief Maximum allowed size of a ring buffer in bytes. */

#pragma once
#include <stdbool.h>
#include <stddef.h>

extern const size_t MAX_ALLOWED_SIZE_BYTES;

typedef struct {
    void *buffer;     // pointer to a buffer's in-memory location
    size_t capacity;  // maximum amount of items in the buffer
    size_t size;      // amount of items currently in the buffer
    size_t item_size; // size of one stored element in bytes
    size_t tail;      // index of the newest (back) element
    size_t head;      // index of the oldest (front) element
} rbuf_t;

/**
 * @brief Creates a new ring buffer.
 *
 * @param capacity Maximum amount of elements that can be stored.
 * @param item_size Size of a single element in bytes.
 * @return The created ring buffer. On failure, returns a buffer with zero
 * `capacity` and `item_size`.
 *
 * @note The total size of the buffer is capped at `MAX_ALLOWED_SIZE_BYTES`.
 */
rbuf_t rbuf_create(size_t capacity, size_t item_size);

/**
 * @brief Allocates a new ring buffer on the heap.
 *
 * @param capacity Maximum amount of elements that can be stored.
 * @param item_size Size of a single element in bytes.
 * @return Pointer to the created ring buffer, or `NULL` on failure.
 *
 * @note The total size of the buffer is capped at `MAX_ALLOWED_SIZE_BYTES`.
 */
rbuf_t *rbuf_allocate(size_t capacity, size_t item_size);

/**
 * @brief Destroys a stack-allocated ring buffer.
 *
 * @param rbuf The target buffer.
 */
void rbuf_destroy(rbuf_t *rbuf);

/**
 * @brief Frees a heap-allocated ring buffer.
 *
 * @param rbuf The target buffer.
 */
void rbuf_free(rbuf_t *rbuf);

/**
 * @brief Returns whether the ring buffer is empty.
 *
 * @param rbuf The target buffer.
 * @return `true` if empty, `false` otherwise.
 */
bool rbuf_is_empty(const rbuf_t *rbuf);

/**
 * @brief Returns a pointer to the newest (back) element in the ring buffer.
 *
 * @param rbuf The target buffer.
 * @return Void pointer to the last element or `NULL` if the buffer is empty.
 */
const void *rbuf_peek_tail(const rbuf_t *rbuf);

/**
 * @brief Returns a pointer to the oldest (front) element in the ring buffer.
 *
 * @param rbuf The target buffer.
 * @return Void pointer to the last element or `NULL` if the buffer is empty.
 */
const void *rbuf_peek_head(const rbuf_t *rbuf);

/**
 * @brief Copies the newest (back) element in the ring buffer to the provided
 * address.
 *
 * @param rbuf The target buffer.
 * @param dest The address, to which the element will be written.
 * @return `true` on success, `false` on failure.
 *
 * @note On failure, the value in `dest` won't be changed.
 */
bool rbuf_copy_tail(const rbuf_t *rbuf, void *restrict dest);

/**
 * @brief Copies the oldest (front) element in the ring buffer to the provided
 * address.
 *
 * @param rbuf The target buffer.
 * @param dest The address, to which the element will be written.
 * @return `true` on success, `false` on failure.
 *
 * @note On failure, the value in `dest` won't be changed.
 */
bool rbuf_copy_head(const rbuf_t *rbuf, void *restrict dest);

/**
 * @brief Adds a new element to the tail (back) of the ring buffer.
 *
 * @param rbuf The target buffer.
 * @param element Pointer to the pushed element.
 */
void rbuf_push_tail(rbuf_t *rbuf, const void *restrict element);

/**
 * @brief Adds a new element to the head (front) of the ring buffer.
 *
 * @param rbuf The target buffer.
 * @param element Pointer to the pushed element.
 */
void rbuf_push_head(rbuf_t *rbuf, const void *restrict element);

/**
 * @brief Removes the newest (back) element from the ring buffer.
 *
 * @param rbuf The target buffer.
 * @return `true` on success, `false` on failure.
 */
bool rbuf_pop_tail(rbuf_t *rbuf);

/**
 * @brief Removes the oldest (front) element from the ring buffer.
 *
 * @param rbuf The target buffer.
 * @return `true` on success, `false` on failure.
 */
bool rbuf_pop_head(rbuf_t *rbuf);