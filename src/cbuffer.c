/**
 * @file util-cbuffer.c
 * @brief Header file for Circular buffer implementation
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org> (v1.0, v2.1)
 * @author Laurens Miers <laurens.miers@mind.be> (v2.0)
 * @version v2.1
 * @date 2020-04-20
 */

#include "cbuffer.h"

static inline int _allocate_internal_buffers(struct cbuffer_t *cbuf)
{
	cbuf->data = malloc(cbuf->nr_elements * sizeof(void *));
	if (!cbuf->data) {
		CBUF_ERR("Failed to create data pointer");
		return -1;
	}
	return 0;
}

struct cbuffer_t *cbuffer_init_cbuffer(int nr_elements)
{
	struct cbuffer_t *cbuf = NULL;

	cbuf = malloc(sizeof(struct cbuffer_t));
	if (!cbuf) {
		CBUF_ERR("Failed to allocate the cbuffer");
		goto error;
	}
	memset(cbuf, 0, sizeof(struct cbuffer_t));
	CBUF_INFO("Buffer created");

	cbuf->nr_elements = nr_elements;
	if (_allocate_internal_buffers(cbuf) < 0) {
		goto error;
	}

	cbuf->wp = cbuf->data;
	cbuf->rp = cbuf->data;

	atomic_init(&cbuf->current_nr_elements, 0);

	return cbuf;
error:
	if (cbuf) {
		free(cbuf);
	}
	return NULL;
}

int cbuffer_signal_element_read(struct cbuffer_t *cbuf)
{
	int error = 0;

	if (!cbuf || !cbuf->rp) {
		CBUF_ERR("RP: cbuffer or cbuffer->wp cannot be NULL!");
		return -1;
	}

#ifdef CBUFFER_VALIDATE_USAGE
	if (cbuf->rp_in_use == false) {
		CBUF_ERR("RP: No read pointer taken!");
		return -1;
	}
	cbuf->rp_in_use = false;
#endif /* CBUFFER_VALIDATE_USAGE */

#ifdef CBUFFER_VALIDATE_PTRS
	CBUF_DEBUG("%p - %p", cbuf->rp, &cbuf->data[cbuf->rp_index]);
	if (cbuf->rp != &cbuf->data[cbuf->rp_index]) {
		CBUF_ERR("RP: Something went wrong with read pointer");
		CBUF_ERR("RP: Either RP is not inline with RP_Index or data ptr has corrupted");
		return -1;
	}
#endif /* CBUFFER_VALIDATE_PTRS */

	if (cbuf->rp == &cbuf->data[cbuf->nr_elements - 1]) {
		CBUF_DEBUG("RP: final element! Moving to start");
		cbuf->rp = cbuf->data;
#ifdef CBUFFER_VALIDATE_PTRS
		cbuf->rp_index = 0;
#endif /* CBUFFER_VALIDATE_PTRS */
	} else {
		CBUF_DEBUG("RP: Incrementing pointer");
		cbuf->rp++;
#ifdef CBUFFER_VALIDATE_PTRS
		cbuf->rp_index++;
#endif /* CBUFFER_VALIDATE_PTRS */
	}

	atomic_fetch_sub(&cbuf->current_nr_elements, 1);

	return error;
}

int cbuffer_signal_element_written(struct cbuffer_t *cbuf)
{
	int error = 0;

	if (!cbuf || !cbuf->wp) {
		CBUF_ERR("WP: cbuffer or cbuffer->wp cannot be NULL!");
		return -1;
	}

#ifdef CBUFFER_VALIDATE_USAGE
	if (cbuf->wp_in_use == false) {
		CBUF_ERR("WP: No write pointer taken!");
		return -1;
	}
	cbuf->wp_in_use = false;
#endif /* CBUFFER_VALIDATE_USAGE */

#ifdef CBUFFER_VALIDATE_PTRS
	CBUF_DEBUG("%p - %p", cbuf->wp, &cbuf->data[cbuf->wp_index]);
	if (cbuf->wp != &cbuf->data[cbuf->wp_index]) {
		CBUF_ERR("WP: Something went wrong with write pointer");
		CBUF_ERR("WP: Either WP is not inline with WP_Index or data ptr has corrupted");
		return -1;
	}
#endif /* CBUFFER_VALIDATE_PTRS */

	if (cbuf->wp == &cbuf->data[cbuf->nr_elements - 1]) {
		CBUF_DEBUG("WP: Final element! Moving to start");
		cbuf->wp = cbuf->data;
#ifdef CBUFFER_VALIDATE_PTRS
		cbuf->wp_index = 0;
#endif /* CBUFFER_VALIDATE_PTRS */
	} else {
		CBUF_DEBUG("WP: Incrementing pointer");
		cbuf->wp++;
#ifdef CBUFFER_VALIDATE_PTRS
		cbuf->wp_index++;
#endif /* CBUFFER_VALIDATE_PTRS */
	}

	atomic_fetch_add(&cbuf->current_nr_elements, 1);

	return error;
}

void cbuffer_flush(struct cbuffer_t *cbuf)
{
	if (!cbuf) {
		return;
	}

	cbuffer_signal_element_read(cbuf);
	cbuffer_signal_element_written(cbuf);

	atomic_store(&cbuf->current_nr_elements, 0);

	cbuf->wp = cbuf->data;
	cbuf->rp = cbuf->data;
}

void cbuffer_destroy_cbuffer(struct cbuffer_t *cbuf)
{
	if (cbuf) {
		/* TODO: Fix this, this is needed when memory is allocated internally */
		/* if (cbuf->data) { */
		/* CBUFFER_DEALLOCATOR_HELPER(cbuf); */
		/* } */
		free(cbuf->data);
		free(cbuf);
	}
}
