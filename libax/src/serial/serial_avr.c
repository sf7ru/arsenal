
#include <serial.h>

#include <uart.h>

#include <util/delay.h>

#include <avr/delay.h>


typedef struct __tag_SERIALDEV
{
    AXDEV               st_dev;

    void (*p_uart_flush)(void);
    uint16_t (*p_uart_available)(void);
    int (*p_uart_putc)(uint8_t data, unsigned int t);
    uint16_t (*p_uart_getc)(void);
    void (*p_uart_init)(uint16_t baudrate);

} SERIALDEV, * PSERIALDEV;


//extern void ledOn(int value);

INT _serial_read(PAXDEV             dev,
                 PVOID              data,
                 INT                size,
                 UINT               TO)
{
    INT             result          = 0;
    unsigned int    ch;
    PU8             on              = data;
    PSERIALDEV      d               = (PSERIALDEV)dev;

    ENTER(true);

    do
    {
        if (!UART_NOT_CHAR(ch = (*d->p_uart_getc)()))
        {
            *(on++) = (U8) ch;
            result++;
        }
        else
        {
            if (!result && TO)
            {
                _delay_ms(1);
                TO--;
            }
            else
                TO = 0;
        }

    } while ((result < size) && TO);

    RETURN(result);
}

INT _serial_write(PAXDEV             dev,
                  PCVOID             data,
                  INT                size,
                  UINT               TO)

{
    INT             result          = 0;
    PSERIALDEV      d               = (PSERIALDEV)dev;
    PU8             on              = data;

    ENTER(true);

    while ((result != size) && ((*d->p_uart_putc)(*on, TO) > 0))
    {
        on++;
        result++;
    }

    RETURN(result);
}

PAXDEV serial_close(PAXDEV dev)
{
    ENTER(true);

    //FREE(dev);

    RETURN(nil);
}

static SERIALDEV    uart0_;
#ifdef USART1_ENABLED
static SERIALDEV    uart1_;
#endif
#ifdef USART2_ENABLED
static SERIALDEV    uart2_;
#endif
#ifdef USART3_ENABLED
static SERIALDEV    uart3_;
#endif



PAXDEV serial_open(PSERIALPROPS p,
                   UINT         type)
{
#define SET_UART(a)             uart##a##_.st_dev.pfn_close      = (PAXDEVFNCLOSE)serial_close;   \
                                uart##a##_.st_dev.pfn_ctl        = (PAXDEVFNCTL) nil;             \
                                uart##a##_.st_dev.pfn_read       = (PAXDEVFNREAD) _serial_read;             \
                                uart##a##_.st_dev.pfn_write      = (PAXDEVFNWRITE) _serial_write;             \
                                uart##a##_.st_dev.pfn_accept     = (PAXDEVFNACCEPT) nil;             \
                                uart##a##_.p_uart_flush   = uart##a##_flush;             \
                                uart##a##_.p_uart_available = uart##a##_available;             \
                                uart##a##_.p_uart_putc    = uart##a##_putc;             \
                                uart##a##_.p_uart_getc    = uart##a##_getc;              \
                                uart##a##_.p_uart_init    = uart##a##_init;              \
                                result                          = &uart##a##_;


    PSERIALDEV      result          = nil;

    ENTER(true);

    switch (p->u_port)
    {
        case 0:
            SET_UART(0);
            break;

#ifdef USART1_ENABLED
        case 1:
            SET_UART(1);
            break;
#endif

#ifdef USART2_ENABLED
        case 1:
            SET_UART(2);
            break;
#endif

#ifdef USART3_ENABLED
        case 1:
            SET_UART(3);
            break;
#endif

        default:
            break;
    }

    if (result)
    {
        (*result->p_uart_init)(UART_BAUD_SELECT(p->u_baudrate * 100, F_CPU));
    }

    RETURN((PAXDEV)result);

#undef  SET_UART
}
