#ifndef REDPILL_VIRTUAL_UART_H
#define REDPILL_VIRTUAL_UART_H

#include <linux/types.h> //bool

/*
 * Length of the RX/TX FIFO in bytes
 * Do NOT change this value just because you want to inject more data at once - it's a hardware-defined property
 */
#define VUART_FIFO_LEN 16

//See vuart_set_tx_callback()
typedef void (vuart_callback_t)(int line, char *buffer, int len);

/**
 * Adds a virtual UART device
 *
 * Calling this function will immediately yank the port from the real one and began capturing its output, so that no
 * data will leave through the real one. However, by itself the data will not be delivered anywhere until you call
 * vuart_set_tx_callback(), which you can do before or after calling vuart_add_device().
 *
 * @param line UART number to replace, e.g. 0 for ttyS0. On systems with inverted UARTs you should use the real one, so
 *             even if ttyS0 points to 2nd physical port this method will ALWAYS use the one corresponding to ttyS*
 *
 * @return 0 on success or -E on error
 */
int vuart_add_device(int line);

/**
 * Removes a virtual UART device
 *
 * Calling this function restores previously replaced port. Unlike vuart_add_device() this function WILL alter TX
 * callbacks by removing all of them. The reasoning behind this is that adding a device and later on adding/changing
 * callbacks makes sense while removing the device and potentially leaving broken pointers can lead to nasty and hard to
 * trace bugs.
 *
 * @param line UART number to replace, e.g. 0 for ttyS0. On systems with inverted UARTs you should use the real one, so
 *             even if ttyS0 points to 2nd physical port this method will ALWAYS use the one corresponding to ttyS*
 *
 * @return 0 on success or -E on error
 */
int vuart_remove_device(int line);

/**
 * Injects data into RX stream of the port
 *
 * It may be confusing at first what's TX and RX in the context here. Imagine a physical chip connected to a computer
 * with some bus (not UART). The chip's RX is what the chip would get from *something*. So injecting data into RX of the
 * chip causes the data to be processed by the chip and arrive in the kernel and then in the application which opened
 * the port. So while TX implies "transmission" from the perspective of the chip and the app opening the port it's an
 * RX side. This naming is consistent with what the whole 8250 subsystem uses.
 *
 * @param line UART number to replace, e.g. 0 for ttyS0. On systems with inverted UARTs you should use the real one, so
 *             even if ttyS0 points to 2nd physical port this method will ALWAYS use the one corresponding to ttyS*
 * @param buffer Pointer to a buffer where we will read from. There's no assumption as to what the buffer contains.
 * @param length Length to read from the buffer up to VUART_FIFO_LEN
 *
 * @return 0 on success or -E on error
 */
int vuart_inject_rx(int line, const char *buffer, int length);

/**
 * Set a function which will be called upon data transmission by the port opener
 *
 * In short you will get data which some app (e.g. cat file > /dev/ttyS0) sent. If you're confused by the RX/TX read the
 * comment for inject_rx().
 *
 * Example of the callback usage:
 *     //The len is the real number of bytes available to read. The buffer ptrs is the same as you gave to set_tx_
 *     void dummy_tx_callback(int line, char *buffer, int len) {
 *         pr_loc_inf("TX @ ttyS%d: |%.*s|", line, len, buffer);
 *     }
 *     //....
 *     char buf[VUART_FIFO_LEN]; //Your buffer should be able to accommodate at least VUART_FIFO_LEN
 *     vuart_set_tx_callback(TRY_PORT, dummy_tx_callback, buf, VUART_FIFO_LEN);
 *
 * WARNING:
 * You callback should be multithreading-aware. It may be called from different contexts. You shouldn't do a lot of work
 * on the thread where your callback has been called. If you need something more copy the buffer and process it on a
 * separate thread.
 *
 * @param line UART number to replace, e.g. 0 for ttyS0. On systems with inverted UARTs you should use the real one, so
 *             even if ttyS0 points to 2nd physical port this method will ALWAYS use the one corresponding to ttyS*
 * @param cb Function to be called; call it with a NULL ptr to remove callback
 * @param buffer A pointer to a buffer where data will be placed. The buffer should be able to accommodate
 *               VUART_FIFO_LEN number of bytes. The buffer you pass will be the same one as passed back during a call
 * @param threshold a *HINT* how many bytes at minimum should be deposited in the FIFO before callback is called. Keep
 *                  in mind that this is just a hint and you callback may be called sooner (e.g. when a client program
 *                  wrote only a single byte using e.g. echo -n X > /dev/ttyS0).
 * @return 0 on success or -E on error
 */
int vuart_set_tx_callback(int line, vuart_callback_t *cb, char *buffer, int threshold);

#endif //REDPILL_VIRTUAL_UART_H
