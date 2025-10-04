#ifndef SF_H
#define SF_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef SF_IMPLEMENTATION
#define SF_EXPORT
#else
#define SF_EXPORT
#endif // SF_IMPLEMENTATION

typedef char byte;
typedef ptrdiff_t size;
typedef uint64_t u64;
typedef uint32_t u32;
typedef int64_t i64;
typedef int32_t i32;
typedef int32_t b32;

#define SF_TRUE 1
#define SF_FALSE 0

#define SF_SIZE(a) (Size)(sizeof(a) / sizeof(0 [a]))
#define SF_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SF_MAX(a, b) ((a) > (b) ? (a) : (b))

struct sf_string_8 {
	size size;
	char const *data;
};

struct sf_queue {
	struct sf_queue *prev;
	struct sf_queue *next;
};

struct sf_arena {
	byte *data;
	u64 position;
	u64 capacity;
	u64 alignment;
};

#define UNUSED(e) (void)(e)

#define SF_OFFSET_OF(ty, f) (ptrdiff_t)(&((ty *)(NULL))->f)
#define SF_CONTAINER_OF(ty, p, f) (ty *)((char *)(p) - SF_OFFSET_OF(ty, f))

#define SF_QUEUE_INIT(q)         \
	do {                     \
		(q)->next = (q); \
		(q)->prev = (q); \
	} while (0)

#define SF_QUEUE_FOR_EACH(q, h) for ((q) = (h)->next; (q) != (h); (q) = (q)->next)

#define SF_QUEUE_INSERT_INTO_HEAD(q, h) \
	do {                            \
		(q)->next = (h)->next;  \
		(q)->prev = (h);        \
		(q)->next->prev = (q);  \
		(h)->next = (q);        \
	} while (0)

#define SF_QUEUE_REMOVE(q)                   \
	do {                                 \
		(q)->prev->next = (q)->next; \
		(q)->next->prev = (q)->prev; \
	} while (0)

#define SF_QUEUE_IS_EMPTY(q) ((q)->next == (q))

#define SF_AS_STRING_8(s, out)            \
	do {                              \
		(out)->data = &(s)[0];    \
		(out)->size = SF_SIZE(s); \
	} while (0)

#define SF_STRING_8_INIT(s)       \
	do {                      \
		(s)->data = NULL; \
		(s)->size = 0;    \
	} while (0)

SF_EXPORT u64 sf_align_u64(u64 size, u64 alignment);
SF_EXPORT void *sf_allocate_memory(u64 size);
SF_EXPORT void sf_free_memory(void *data);

SF_EXPORT void sf_arena_init(u64 capactiy, u64 alignment, struct sf_arena *arena);
SF_EXPORT void sf_arena_deinit(struct sf_arena *arena);
SF_EXPORT void *sf_allocate(struct sf_arena *arena, u64 size);

SF_EXPORT void sf_load_file_into_string_8(struct sf_arena *arena, struct sf_string_8 *path, struct sf_string_8 *out);

SF_EXPORT b32 sf_compare_string_8(struct sf_string_8 const *lhs, struct sf_string_8 const *rhs);

SF_EXPORT void sf_null_terminate_string_8(struct sf_arena *arena, struct sf_string_8 const *s, struct sf_string_8 *out);

#ifdef SF_IMPLEMENTATION

SF_EXPORT u64 sf_align_u64(u64 value, u64 alignment) {
	return (value + alignment - 1) & ~(alignment - 1);
}

SF_EXPORT void *sf_allocate_memory(u64 size) {
	return malloc(size);
}

SF_EXPORT void sf_free_memory(void *data) {
	free(data);
}

SF_EXPORT void sf_arena_init(u64 capacity, u64 alignment, struct sf_arena *arena) {
	arena->data = sf_allocate_memory(capacity);
	if (arena->data) {
		arena->position = 0;
		arena->capacity = capacity;
		arena->alignment = alignment;
	} else {
		arena->position = 0;
		arena->capacity = 0;
		arena->alignment = 0;
	}
}

SF_EXPORT void sf_arena_deinit(struct sf_arena *arena) {
	sf_free_memory(arena->data);
	arena->data = NULL;
	arena->position = 0;
	arena->capacity = 0;
	arena->alignment = 0;
}

SF_EXPORT void *sf_allocate(struct sf_arena *arena, u64 size) {
	byte *data = NULL;
	u64 i = 0, prev = 0, req = arena->position + size;

	if (req > arena->capacity)
		return NULL;

	prev = arena->position;
	arena->position = sf_align_u64(req, arena->alignment);

	data = &arena->data[prev];
	for (i = 0; i < size; ++i)
		data[i] = 0xD1;

	return data;
}

SF_EXPORT void sf_load_file_into_string_8(struct sf_arena *arena, struct sf_string_8 *path, struct sf_string_8 *out) {
	FILE *fd = NULL;
	long buf_size = 0;
	size_t req_size = 0;
	byte *data = 0;
	struct sf_string_8 null_terminated_path;

	SF_STRING_8_INIT(&null_terminated_path);
	SF_STRING_8_INIT(out);

	sf_null_terminate_string_8(arena, path, &null_terminated_path);
	if (!null_terminated_path.data)
		goto cleanup;

	fd = fopen(null_terminated_path.data, "r");
	if (!fd)
		goto cleanup;

	if (0 != fseek(fd, 0L, SEEK_END))
		goto cleanup;

	buf_size = ftell(fd);
	if (-1 == buf_size)
		goto cleanup;

	data = sf_allocate(arena, buf_size);
	if (!data)
		goto cleanup;

	if (0 != fseek(fd, 0L, SEEK_SET))
		goto cleanup;

	req_size = fread(data, sizeof(char), buf_size, fd);
	if (ferror(fd))
		goto cleanup;

	out->data = data;
	out->size = req_size;

cleanup:
	if (fd)
		fclose(fd);
}

SF_EXPORT b32 sf_compare_string_8(struct sf_string_8 const *lhs, struct sf_string_8 const *rhs) {
	size i = 0;

	if (lhs->size != rhs->size)
		return SF_FALSE;

	for (i = 0; i < SF_MIN(lhs->size, rhs->size); ++i)
		if (lhs->data[i] != rhs->data[i])
			return SF_FALSE;

	return SF_TRUE;
}

SF_EXPORT void sf_null_terminate_string_8(struct sf_arena *arena, struct sf_string_8 const *s, struct sf_string_8 *out) {
	byte *data = NULL;

	SF_STRING_8_INIT(out);

	data = sf_allocate(arena, s->size + 1);
	if (data) {
		size i = 0;

		for (i = 0; i < s->size; ++i)
			data[i] = s->data[i];

		data[s->size] = '\0';

		out->data = data;
		out->size = s->size;
	}
}

#endif // SF_IMPLEMENTATION

#endif // SF_H
