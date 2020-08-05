#include <string.h>

// Writer
bool enqueue(bltpool_t *bltpool, void (*func)(arg_t []), unsigned n, ...)
{
  bltpool_job_t job = { .func = func, .nargs = n };
  va_list valist;
  va_start(valist, n);

  for (unsigned i = 0; i < n; i++)
  {
    arg_t arg = va_arg(valist, arg_t);

    if (arg.size)
    {
      void *data;
      
      if ((data = malloc(arg.size)) == NULL)
      {
        for (unsigned j = 0; j < i - 1; j++)
          if (job.ARG[j].size)
            free(job.ARG[j].arg);

        return 0;
      }

      memcpy(data, arg.arg, arg.size);
      arg.arg = data;
    }
    
    job.ARG[i] = arg;
  }
 
  for (unsigned i = n; i < MAX_ARGS; i++)
    job.ARG[i] = (arg_t) { NULL, 0 };

  va_end(valist);
  pthread_mutex_lock(&bltpool->mutex);
  
  if (bl_add(&bltpool->Q, &job))
  {
    pthread_mutex_unlock(&bltpool->mutex);
    pthread_cond_signal(&bltpool->cond_var);
  }

  else
  {
    for (unsigned i = 0; i < n; i++)
      if (job.ARG[i].size)
        free(job.ARG[i].arg);

    pthread_mutex_unlock(&bltpool->mutex);
    return 0;
  }

  return 1;
}

// Reader
static void *worker_th(void *userp)
{
  bltpool_t *bltpool = userp;

  while (1)
  {
    pthread_mutex_lock(&bltpool->mutex);

    while (!bl_count(bltpool->Q) && !bltpool->quit)
      pthread_cond_wait(&bltpool->cond_var, &bltpool->mutex);

    if (bltpool->quit)
    {
      pthread_mutex_unlock(&bltpool->mutex);
      return NULL;
    }
    /* Need to make a copy of head job because the location 
     * of the list will change as the list is realloc'd when
     * new elements are added to it.
     */
    bltpool_job_t *job = bl_head(bltpool->Q), 
                  localjob = { .func = job->func };

    for (unsigned i = 0; i < MAX_ARGS; i++)
      localjob.ARG[i] = job->ARG[i];

    pthread_mutex_unlock(&bltpool->mutex);
    localjob.func(localjob.ARG);
    pthread_mutex_lock(&bltpool->mutex);
    queue_pop(bltpool);
    pthread_mutex_unlock(&bltpool->mutex);
  }

  return NULL;
}

void squeue_del(squeue_t *squeue)
{
  pthread_join(squeue->pth, NULL);
  free(squeue);
  squeue = NULL;
}

squeue_t *squeue_new(void)
{
  squeue_t *squeue = calloc(1, sizeof *squeue);

  if (squeue == NULL)
    return NULL;

  pthread_create(&squeue->pth, NULL, worker_th, (void *) squeue);
  return squeue;
}
