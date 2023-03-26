// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#include <ringbuff.h>
#include <string.h>

#define BUF_MEMSET                      memset
#define BUF_MEMCPY                      memcpy

PRINGBUFF ringbuff_create(UINT               size)
{
    PRINGBUFF  result          = nil;

    ENTER(true);

    if ((result = CREATE_X(RINGBUFF, size + 1)) != nil)
    {
        result->size        = size;
    }

    RETURN(result);
}
void ringbuff_clean(PRINGBUFF        b)
{
    b->head = b->tail = 0;
}

BOOL ringbuff_is_empty(PRINGBUFF cbuf)
{
    return (cbuf->head == cbuf->tail);
}

PRINGBUFF ringbuff_destroy(PRINGBUFF          b)
{
    SAFEFREE(b);
    return nil;
}
size_t
ringbuff_get_free(PRINGBUFF buff)
{
    size_t size, w, r;

    /* Operate on temporary values in case they change in between */
    w = buff->tail;
    r = buff->head;
    if (w == r) {
        size = buff->size;
    } else if (r > w) {
        size = r - w;
    } else {
        size = buff->size - (w - r);
    }

    /* Buffer free size is always 1 less than actual size */
    return size - 1;
}
size_t ringbuff_get_full(PRINGBUFF buff)
{
    size_t w, r, size;

    /* Operate on temporary values in case they change in between */
    w = buff->tail;
    r = buff->head;
    if (w == r) {
        size = 0;
    } else if (w > r) {
        size = w - r;
    } else {
        size = buff->size - (r - w);
    }
    return size;
}


INT ringbuff_write(PRINGBUFF          buff,
                   PVOID              p_data,
                   INT                count)

//size_t
//ringbuff_write(ringbuff_t* buff, const void* data, size_t count)
{
    INT             tocopy;
    INT             free;
    PCU8            d = p_data;


    if (buff->tail >= buff->size)
    {                /// Check input pointer
        buff->tail = 0;                            // On normal use, this should never happen
    }

    /* Calculate maximum number of bytes we can write */
    free = ringbuff_get_free(buff);
    if (free < count) {
        if (free == 0) {
            return 0;
        }
        count = free;
    }

    /* Write data to linear part of buffer */
    tocopy = buff->size - buff->tail;              /* Calculate number of elements we can put at the end of buffer */
    if (tocopy > count) {
        tocopy = count;
    }
    BUF_MEMCPY(&buff->data[buff->tail], d, tocopy);
    buff->tail += tocopy;
    count -= tocopy;

    /* Write data to overflow part of buffer */
    if (count > 0) {
        BUF_MEMCPY(buff->data, (void *)&d[tocopy], count);
        buff->tail = count;
    }
    if (buff->tail >= buff->size) {                /* Check input overflow */
        buff->tail = 0;
    }
    return tocopy + count;                      /* Return number of elements stored in memory */
}

INT ringbuff_read(PRINGBUFF         buff,
                  PVOID              data,
                  INT                count)
{
    size_t tocopy, full;
    PU8 d = data;


    if (buff->head >= buff->size) {                /* Check output pointer */
        buff->head = 0;                            /* On normal use, this should never happen */
    }

    /* Calculate maximum number of bytes we can read */
    full = ringbuff_get_full(buff);
    if (full < count) {
        if (full == 0) {
            return 0;
        }
        count = full;
    }

    /* Read data from linear part of buffer */
    tocopy = buff->size - buff->head;
    if (tocopy > count) {
        tocopy = count;
    }
    BUF_MEMCPY(d, &buff->data[buff->head], tocopy);
    buff->head += tocopy;
    count -= tocopy;

    /* Read data from overflow part of buffer */
    if (count > 0) {
        BUF_MEMCPY(&d[tocopy], buff->data, count);
        buff->head = count;
    }
    if (buff->head >= buff->size) {                /* Check output overflow */
        buff->head = 0;
    }
    return tocopy + count;                      /* Return number of elements stored in memory */
}

/*
///
// \brief           Read from buffer but do not change read pointer
// \param[in]       buff: Buffer handle
// \param[in]       skip_count: Number of bytes to skip before reading data
// \param[out]      data: Pointer to data to save read memory
// \param[in]       count: Number of bytes to peek
// \return          Number of bytes written to data array
//
size_t
ringbuff_peek(ringbuff_t* buff, size_t skip_count, void* data, size_t count) {
    size_t full, tocopy, r;
    uint8_t *d = data;

    if (buff == NULL || buff->buff == NULL || count == 0) {
        return 0;
    }

    if (buff->head >= buff->size) {                // Check output pointer 
        buff->head = 0;                            // On normal use, this should never happen 
    }
    r = buff->head;

    // Calculate maximum number of bytes we can read 
    full = ringbuff_get_full(buff);

    // Skip beginning of buffer 
    if (skip_count >= full) {
        return 0;
    }
    r += skip_count;
    full -= skip_count;
    if (r >= buff->size) {
        r -= buff->size;
    }

    // Check if we can read something after skip 
    if (full < count) {
        if (full == 0) {
            return 0;
        }
        count = full;
    }

    // Read data from linear part of buffer 
    tocopy = buff->size - r;
    if (tocopy > count) {
        tocopy = count;
    }
    BUF_MEMCPY(d, &buff->buff[r], tocopy);
    count -= tocopy;

    // Read data from overflow part of buffer 
    if (count > 0) {
        BUF_MEMCPY(&d[tocopy], buff->buff, count);
    }
    return tocopy + count;                      // Return number of elements stored in memory 
}

///
// \brief           Get number of bytes in buffer available to write
// \param[in]       buff: Buffer handle
// \return          Number of free bytes in memory
///
size_t
ringbuff_get_free(ringbuff_t* buff) {
    size_t size, w, r;

    if (buff == NULL || buff->buff == NULL) {
        return 0;
    }

    // Operate on temporary values in case they change in between 
    w = buff->tail;
    r = buff->head;
    if (w == r) {
        size = buff->size;
    } else if (r > w) {
        size = r - w;
    } else {
        size = buff->size - (w - r);
    }

    // Buffer free size is always 1 less than actual size 
    return size - 1;
}

///
 * \brief           Get number of bytes in buffer available to read
 * \param[in]       buff: Buffer handle
 * \return          Number of bytes ready to be read
 
size_t
ringbuff_get_full(ringbuff_t* buff) {
    size_t w, r, size;

    if (buff == NULL || buff->buff == NULL) {
        return 0;
    }

    // Operate on temporary values in case they change in between 
    w = buff->tail;
    r = buff->head;
    if (w == r) {
        size = 0;
    } else if (w > r) {
        size = w - r;
    } else {
        size = buff->size - (r - w);
    }
    return size;
}

///
 * \brief           Resets and clears buffer
 * \param[in]       buff: Buffer handle
 
void
ringbuff_reset(ringbuff_t* buff) {
    if (buff == NULL) {
        return;
    }
    buff->tail = 0;
    buff->head = 0;
}

///
 * \brief           Get linear address for buffer for fast read
 * \param[in]       buff: Buffer handle
 * \return          Linear buffer start address
 
void *
ringbuff_get_linear_block_read_address(ringbuff_t* buff) {
    if (buff == NULL || buff->buff == NULL) {
        return NULL;
    }
    return &buff->buff[buff->head];
}

///
 * \brief           Get length of linear block address before it overflows for read operation
 * \param[in]       buff: Buffer handle
 * \return          Linear buffer size in units of bytes for read operation
 
size_t
ringbuff_get_linear_block_read_length(ringbuff_t* buff) {
    size_t w, r, len;

    if (buff == NULL) {
        return 0;
    }

    // Operate on temporary values in case they change in between 
    w = buff->tail;
    r = buff->head;
    if (w > r) {
        len = w - r;
    } else if (r > w) {
        len = buff->size - r;
    } else {
        len = 0;
    }
    return len;
}

///
 * \brief           Skip (ignore, advance read pointer) buffer data.
 * \note            Useful at the end of streaming transfer such as DMA
 * \param[in]       buff: Buffer handle
 * \param[in]       len: Number of bytes to skip
 * \return          Number of bytes skipped
 
size_t
ringbuff_skip(ringbuff_t* buff, size_t len) {
    size_t full;

    if (buff == NULL || len == 0) {
        return 0;
    }

    full = ringbuff_get_full(buff);             // Get buffer used length 
    if (len > full) {
        len = full;
    }
    buff->head += len;                             // Advance read pointer 
    if (buff->head >= buff->size) {                // Subtract possible overflow 
        buff->head -= buff->size;
    }
    return len;
}

///
 * \brief           Get linear address for buffer for fast read
 * \param[in]       buff: Buffer handle
 * \return          Linear buffer start address
 
void *
ringbuff_get_linear_block_write_address(ringbuff_t* buff) {
    if (buff == NULL || buff->buff == NULL) {
        return NULL;
    }
    return &buff->buff[buff->tail];
}

///
 * \brief           Get length of linear block address before it overflows for write operation
 * \param[in]       buff: Buffer handle
 * \return          Linear buffer size in units of bytes for write operation
 
size_t
ringbuff_get_linear_block_write_length(ringbuff_t* buff) {
    size_t w, r, len;

    if (buff == NULL) {
        return 0;
    }

    // Operate on temporary values in case they change in between 
    w = buff->tail;
    r = buff->head;
    if (w >= r) {
        len = buff->size - w;
        //
         * When read pointer == 0,
         * maximal length is one less as if too many bytes 
         * are written, it buffer would be considered empty again (r == w)
         
        if (r == 0) {
            len--;
        }
    } else {
        len = r - w - 1;
    }
    return len;
}

///
 * \brief           Advance write pointer in the buffer
 * \note            Useful when hardware is writing to buffer and app needs to increase number of bytes written
 * \param[in]       buff: Buffer handle
 * \param[in]       len: Number of bytes to advance
 * \return          Number of bytes advanced for write operation
 
size_t
ringbuff_advance(ringbuff_t* buff, size_t len)
{
    size_t free;

    if (buff == NULL || len == 0)
    {
        return 0;
    }

    free = ringbuff_get_free(buff);             // Get buffer free length 
    if (len > free)
    {
        len = free;
    }
    buff->tail += len;                             // Advance write pointer 
    if (buff->tail >= buff->size)
    {                // Subtract possible overflow 
        buff->tail -= buff->size;
    }
    return len;
}
*/
