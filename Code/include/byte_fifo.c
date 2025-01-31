#include "byte_fifo.h"





void queue_byte_init(STU_BYTE_QUEUE *Queue,uint8_t *pbuf,uint16_t buf_len)
{
    Queue->in=0;
    Queue->out=0;
    Queue->max_buf=buf_len;
    Queue->buf=pbuf;
}


/*
return : 
true: queue is empty
false: queue have data
*/

bool queue_byte_is_empty(STU_BYTE_QUEUE *Queue)
{
    if(Queue->in==Queue->out)
    return true;
    else
    return false;
}


/*
return : 
true: queue empty
false: queue full
*/

bool queue_byte_in(STU_BYTE_QUEUE *Queue,uint8_t datain)
{   

    bool ret=true;


    if(((Queue->in+1)%Queue->max_buf)==Queue->out)//queue is full
    {
    ret=false;
    }

    Queue->buf[Queue->in] = datain;
    Queue->in++;
    Queue->in%=Queue->max_buf;
    return ret;


}

/*
para:   pdata--------->point out data

return : 
true:have data in queue
false:no data in queue
*/

bool queue_byte_out(STU_BYTE_QUEUE *Queue,uint8_t *pdata)
{
 
    if(Queue->in == Queue->out) 
    {
        return false;
    }
    else
    {
        *pdata = Queue->buf[Queue->out];
        Queue->out++;
        Queue->out%=Queue->max_buf;
      
        return true;
    }
}