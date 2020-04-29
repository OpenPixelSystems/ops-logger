#include "queue.h"

struct queue_t *queue_create(queue_destroy_t destroy)
{
	struct queue_t *q = malloc(sizeof(struct queue_t));

	if (!q) {
		return NULL;
	}
	memset(q, 0, sizeof(struct queue_t));
	q->destroy = destroy;

	LOG_INFO("Queue created");

	return q;
}

int queue_push(struct queue_t *q, void *data)
{
	if (!q) {
		return -1;
	}

	struct queue_elm_t *new = malloc(sizeof(struct queue_elm_t));
	if (!new) {
		return -1;
	}
	memset(new, 0, sizeof(struct queue_elm_t));

	pthread_mutex_lock(&q->mutex);
	new->data = data;

	if (q->n_elements == 0) {
		new->id = 0;
		q->head = new;
		q->tail = new;
	} else {
		new->id = q->tail->id++;
		q->tail->next = new;
		new->prev = q->tail;
		q->tail = new;
	}
	q->n_elements++;

	pthread_mutex_unlock(&q->mutex);

	return 0;
}

int queue_pop(struct queue_t *q, void **data)
{
	struct queue_elm_t *tmp, *new_head;

	if (!q) {
		return -1;
	}

	pthread_mutex_lock(&q->mutex);

	if (q->n_elements == 0) {
		pthread_mutex_unlock(&q->mutex);
		return -1;
	}

	tmp = q->head;
	if (!tmp) {
		LOG_DEBUG("%d TMP is NULL!!", q->n_elements);
	}

	if (q->n_elements > 1) {
		new_head = tmp->next;
		new_head->prev = NULL;
		q->head = new_head;
	} else {
		q->head = NULL;
		q->tail = NULL;
	}

	q->n_elements--;

	// LOG_DEBUG("Dequeued buffer %d",tmp->id);

	*data = tmp->data;
	free(tmp);

	pthread_mutex_unlock(&q->mutex);

	return 0;
}

void queue_destroy(struct queue_t *q)
{
	void *data;

	if (!q) {
		return;
	}

	while (QUEUE_SIZE(q)) {
		queue_pop(q, &data);
		if (q->destroy) {
			q->destroy(data);
		}
	}
	free(q);
	q = NULL;
	return;
}
