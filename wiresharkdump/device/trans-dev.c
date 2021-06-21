#include <drivers/uart.h>
#include "misc_evt.h"
#include "wiresharkdump.h"

void test_single_read_callback(const struct device *dev,
							   struct uart_event *evt, void *user_data)
{
	ARG_UNUSED(dev);

	switch (evt->type)
	{
	case UART_TX_DONE:
		wsk_frame_tx_done();
		break;
	default:
		break;
	}
}

struct device *wsk_trans_dev_init(char *dev_name)
{
	const struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE};

	const struct device *uart_dev = device_get_binding(dev_name);

	/* Verify configure() - set device configuration using data in cfg */
	int ret = uart_configure(uart_dev, &uart_cfg);
	if (ret == -ENOTSUP)
	{
		return 0;
	}

	uart_callback_set(uart_dev,
					  test_single_read_callback,
					  NULL);

	return uart_dev;
}