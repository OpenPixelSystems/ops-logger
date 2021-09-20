/**
 * @file cbuffer.h
 * @brief Header file for Circular buffer implementation
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org> (v1.0, v2.1)
 * @author Laurens Miers <laurens.miers@mind.be> (v2.0)
 * @version v2.1
 * @date 2020-04-20
 */

/**
 * Notable changes with v2.0:
 * - Write / Read pointer validity no longer depends on current_nr_elements.
 *   While current_nr_elements is still susceptive to race-conditions it no longer
 *   impacts the read/write pointers
 * - Check pointer validity and wp/rp usages (check that no new pointer can be
 *   taken when the previous one has not been signaled as read or written)
 */

#ifndef _CBUFFER_H_
#define _CBUFFER_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <stdatomic.h>

/* #define CBUFFER_DEBUG_OUTPUT */
/* #define CBUFFER_VALIDATE_PTRS */
/* #define CBUFFER_VALIDATE_USAGE */

#define CBUF_INFO(msg, ...) \
	printf("(INFO) %s: %s: (%d) " msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

#define CBUF_ERR(msg, ...) \
	printf("(ERROR) %s: %s: (%d) " msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

#ifdef CBUFFER_DEBUG_OUTPUT
#define CBUF_DEBUG(msg, ...) \
	printf("(DEBUG) " msg "\n", ## __VA_ARGS__)
#else
#define CBUF_DEBUG(msg, ...) while (0) {};
#endif

/**
 * @brief Cbuffer data structure
 */
struct cbuffer_t {
	int	nr_elements;            //!< Number of elements available
	atomic_int	current_nr_elements;    //!< Current Number of elements

	void **		rp;                     //!< Current read pointer
	void **		wp;                     //!< Current write pointer

#ifdef CBUFFER_VALIDATE_USAGE
	bool		rp_in_use;      //!< Is a write pointer in use?
	bool		wp_in_use;      //!< Is a read pointer in use?
#endif /* CBUFFER_VALIDATE_USAGE */

#ifdef CBUFFER_VALIDATE_PTRS
	uint8_t		rp_index;       //!< Current wp index in data
	uint8_t		wp_index;       //!< Current rp index in data
#endif /* CBUFFER_VALIDATE_PTRS */

	void **		data; //!< The actual data elements
};


/**
 * @brief  Initialize the cbuffer
 *
 * @param nr_elements Number of elements the buffer should hold
 *
 * @returns  NULL if failed, otherwise an allocated cbuffer
 */
struct cbuffer_t *cbuffer_init_cbuffer(int nr_elements);

/**
 * @brief  Retrieve the cbuffer size
 *
 * @param cbuf Cbuffer of which we retrieve the size
 *
 * @returns  The size or -1 if failed
 */
static inline int cbuffer_get_size(struct cbuffer_t *cbuf)
{
	if (cbuf) {
		return cbuf->nr_elements;
	}
	return -1;
}

/**
 * @brief  Retrieve the cbuffer count
 *
 * @param cbuf Cbuffer of which we retrieve the count
 *
 * @returns  The size or -1 if failed
 */
static inline int cbuffer_get_count(struct cbuffer_t *cbuf)
{
	if (cbuf) {
		if (atomic_load(&cbuf->current_nr_elements) < 0) {
			/* Recover from previously commited crimes.. don't worry */
			atomic_store(&cbuf->current_nr_elements, 0);
		}
		return atomic_load(&cbuf->current_nr_elements);
	}
	return -1;
}

/**
 * @brief  Retrieve the current read pointer
 *
 * @param cbuf The cbuffer of which the pointer will be retrieved
 *
 * @returns  NULL if failed, otherwise a valid data pointer
 */
static inline void *cbuffer_get_read_pointer(struct cbuffer_t *cbuf)
{
	if (!cbuf || !cbuf->rp) {
		CBUF_ERR("Invalid argument, cbuf || cbuf->rp == NULL");
		return NULL;
	}

	if (!cbuffer_get_count(cbuf)) {
		return NULL;
	}

#ifdef CBUFFER_VALIDATE_USAGE
	if (cbuf->rp_in_use == true) {
		CBUF_ERR("RP Already taken!");
		return NULL;
	}
	cbuf->rp_in_use = true;
#endif /* CBUFFER_VALIDATE_USAGE */

	return *cbuf->rp;
}

/**
 * @brief  Retrieve the current write pointer
 *
 * @param cbuf The cbuffer of which the pointer will be retrieved
 *
 * @returns  NULL if failed, otherwise a valid data pointer
 */
static inline void *cbuffer_get_write_pointer(struct cbuffer_t *cbuf)
{
	if (!cbuf || !cbuf->wp) {
		CBUF_ERR("Invalid argument, cbuf || cbuf->wp == NULL");
		return NULL;
	}

	if (cbuffer_get_count(cbuf) == cbuf->nr_elements) {
		return NULL;
	}

#ifdef CBUFFER_VALIDATE_USAGE
	if (cbuf->wp_in_use == true) {
		CBUF_ERR("WP Already taken!");
		return NULL;
	}
	cbuf->wp_in_use = true;
#endif /* CBUFFER_VALIDATE_USAGE */

	return *cbuf->wp;
}

/**
 * @brief  Retrieve the current raw read pointer
 *
 * @param cbuf The cbuffer of which the pointer will be retrieved
 *
 * @returns  NULL if failed, otherwise a valid pointer to the data pointer
 */
static inline void **cbuffer_get_raw_read_pointer(struct cbuffer_t *cbuf)
{
	if (!cbuf || !cbuf->rp) {
		CBUF_ERR("Invalid argument, cbuf || cbuf->rp == NULL");
		return NULL;
	}

	return cbuf->rp;
}

/**
 * @brief  Retrieve the current raw write pointer
 *
 * @param cbuf The cbuffer of which the pointer will be retrieved
 *
 * @returns  NULL if failed, otherwise a valid pointer to the data pointer
 */
static inline void **cbuffer_get_raw_write_pointer(struct cbuffer_t *cbuf)
{
	if (!cbuf || !cbuf->wp) {
		CBUF_ERR("Invalid argument, cbuf || cbuf->wp == NULL");
		return NULL;
	}
	return cbuf->wp;
}

/**
 * @brief  Signal that an element was read
 *
 * @param cbuf The cbuffer to which we signal this
 *
 * @returns   -1 if failed otherwise 0
 */
int cbuffer_signal_element_read(struct cbuffer_t *cbuf);

/**
 * @brief  Signal that an element was written
 *
 * @param cbuf The cbuffer to which we signal this
 *
 * @returns   -1 if failed otherwise 0
 */
int cbuffer_signal_element_written(struct cbuffer_t *cbuf);

void cbuffer_flush(struct cbuffer_t *cbuf);

static inline int cbuffer_set_element(struct cbuffer_t *cbuf, ssize_t index, void *element)
{
	if (!cbuf) {
		return -1;
	}

	if (index >= cbuf->nr_elements) {
		/* index out of range */
		return -1;
	}

	cbuf->data[index] = element;
	return 0;
}

static inline void *cbuffer_get_element(struct cbuffer_t *cbuf, ssize_t index)
{
	if (!cbuf) {
		return NULL;
	}
	if (index >= cbuf->nr_elements) {
		/* index out of range */
		return NULL;
	}

	return cbuf->data[index];
}

/**
 * @brief  Destroy a given cbuffer
 *
 * @param cbuf The cbuffer that will be cleaned
 */
void cbuffer_destroy_cbuffer(struct cbuffer_t *cbuf);

#define cbuffer_init(x) cbuffer_init_cbuffer(x)
#define cbuffer_flush_all(x) cbuffer_flush(x)
#define cbuffer_destroy(x) cbuffer_destroy_cbuffer(x)

#define CBUFFER_ALLOCATOR_HELPER(cbuf, type) \
	for (int iter = 0; iter < (cbuf)->nr_elements; iter++) { \
		(cbuf)->data[iter] = malloc(sizeof(type)); \
		memset((cbuf)->data[iter], 0, sizeof(type)); \
	}

#define CBUFFER_DEALLOCATOR_HELPER(cbuf) \
	for (int iter = 0; iter < (cbuf)->nr_elements; iter++) { \
		if ((cbuf)->data[iter]) { \
			free((cbuf)->data[iter]); \
		} \
	}
#endif /* _CBUFFER_H_ */

