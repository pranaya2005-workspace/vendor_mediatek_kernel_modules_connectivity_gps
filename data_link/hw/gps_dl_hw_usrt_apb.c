#include "gps_dl_config.h"
#include "gps_dl_context.h"
#include "gps_dl_time_tick.h"

#include "gps_dl_hal.h"
#if GPS_DL_MOCK_HAL
#include "gps_mock_hal.h"
#endif

#include "gps_dl_hw_api.h"
#include "gps_dl_hw_priv_util.h"

#include "gps/gps_usrt_apb.h"
#include "gps/gps_l5_usrt_apb.h"
#include "gps/bgf_gps_dma.h"
#include "conn_infra/conn_host_csr_top.h"

void gps_dl_hw_usrt_rx_irq_enable(enum gps_dl_link_id_enum link_id, bool enable)
{
	if (enable)
		GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_INTEN_TXIEN, GPS_L5_USRT_APB_APB_INTEN_TXIEN);
	else
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_INTEN_TXIEN, GPS_L5_USRT_APB_APB_INTEN_TXIEN);
}

void gps_dl_hw_usrt_ctrl(enum gps_dl_link_id_enum link_id,
	bool is_on, bool is_dma_mode, bool is_1byte_mode)
{
	if (is_1byte_mode)
		GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_CTRL_BYTEN, GPS_L5_USRT_APB_APB_CTRL_BYTEN);
	else
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_CTRL_BYTEN, GPS_L5_USRT_APB_APB_CTRL_BYTEN);

	if (!is_on) {
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_CTRL_TX_EN, GPS_L5_USRT_APB_APB_CTRL_TX_EN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_CTRL_RX_EN, GPS_L5_USRT_APB_APB_CTRL_RX_EN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_INTEN_TXIEN, GPS_L5_USRT_APB_APB_INTEN_TXIEN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_INTEN_NODAIEN, GPS_L5_USRT_APB_APB_INTEN_NODAIEN);
	} else if (is_dma_mode) {
		GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_CTRL_TX_EN, GPS_L5_USRT_APB_APB_CTRL_TX_EN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_CTRL_RX_EN, GPS_L5_USRT_APB_APB_CTRL_RX_EN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_INTEN_TXIEN, GPS_L5_USRT_APB_APB_INTEN_TXIEN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_INTEN_NODAIEN, GPS_L5_USRT_APB_APB_INTEN_NODAIEN);
	} else {
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_CTRL_TX_EN, GPS_L5_USRT_APB_APB_CTRL_TX_EN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_CTRL_RX_EN, GPS_L5_USRT_APB_APB_CTRL_RX_EN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_INTEN_TXIEN, GPS_L5_USRT_APB_APB_INTEN_TXIEN);
		GDL_HW_SET_GPS_ENTRY2(link_id, 0, GPS_USRT_APB_APB_INTEN_NODAIEN, GPS_L5_USRT_APB_APB_INTEN_NODAIEN);
	}

	/* not speed up */
	if (is_1byte_mode)
		GDL_HW_SET_GPS_ENTRY2(link_id, 0x4, GPS_USRT_APB_MCU_A2D1_A2D_1, GPS_L5_USRT_APB_MCU_A2D1_A2D_1);
	else
		GDL_HW_SET_GPS_ENTRY2(link_id, 0x6, GPS_USRT_APB_MCU_A2D1_A2D_1, GPS_L5_USRT_APB_MCU_A2D1_A2D_1);

	GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_MCUB_A2DF_A2DF3, GPS_L5_USRT_APB_MCUB_A2DF_A2DF3);

	/* wait ROM okay flag */
	if (link_id == GPS_DATA_LINK_ID0)
		GDL_HW_POLL_GPS_ENTRY(GPS_USRT_APB_MCUB_D2AF_D2AF3, 1, POLL_DEFAULT);
	else
		GDL_HW_POLL_GPS_ENTRY(GPS_L5_USRT_APB_MCUB_D2AF_D2AF3, 1, POLL_DEFAULT);
}

bool gps_dl_hw_usrt_has_set_nodata_flag(enum gps_dl_link_id_enum link_id)
{
	return (bool)GDL_HW_GET_GPS_ENTRY2(link_id,
		GPS_USRT_APB_APB_STA_NODAINTB, GPS_L5_USRT_APB_APB_STA_NODAINTB);
}

void gps_dl_hw_usrt_clear_nodata_irq(enum gps_dl_link_id_enum link_id)
{
	GDL_HW_SET_GPS_ENTRY2(link_id, 1, GPS_USRT_APB_APB_STA_NODAINTB, GPS_L5_USRT_APB_APB_STA_NODAINTB);
}

enum GDL_RET_STATUS gps_dl_hal_wait_and_handle_until_usrt_has_data(
	enum gps_dl_link_id_enum link_id, int timeout_usec)
{
	struct gps_dl_hw_usrt_status_struct usrt_status;
	bool last_rw_log_on;
	unsigned int tick0, tick1;

	tick0 = gps_dl_tick_get();

	if (gps_dl_show_reg_wait_log())
		GDL_LOGXD(link_id, "timeout = %d", link_id, timeout_usec);

	while (1) {
		gps_dl_hw_save_usrt_status_struct(link_id, &usrt_status);

		if (gps_dl_only_show_wait_done_log())
			last_rw_log_on = gps_dl_set_show_reg_rw_log(false);
		else
			gps_dl_hw_print_usrt_status_struct(link_id, &usrt_status);

		if (GDL_HW_EXTRACT_ENTRY(GPS_USRT_APB_APB_STA_TX_IND, usrt_status.state)) {
			if (gps_dl_only_show_wait_done_log()) {
				gps_dl_set_show_reg_rw_log(last_rw_log_on);
				gps_dl_hw_print_usrt_status_struct(link_id, &usrt_status);
				gps_dl_hw_save_usrt_status_struct(link_id, &usrt_status);
			}

			gps_dl_hal_event_send(GPS_DL_HAL_EVT_D2A_RX_HAS_DATA, link_id);
			break;
		}

		tick1 = gps_dl_tick_get();
		if (timeout_usec > GPS_DL_RW_NO_TIMEOUT &&
			gps_dl_tick_delta_to_usec(tick0, tick1) >= timeout_usec)
			return GDL_FAIL_TIMEOUT;

		gps_dl_wait_us(GDL_HW_STATUS_POLL_INTERVAL_USEC);
	}

	return GDL_OKAY;
}

enum GDL_RET_STATUS gps_dl_hal_wait_and_handle_until_usrt_has_nodata_or_rx_dma_done(
	enum gps_dl_link_id_enum link_id, int timeout_usec)
{
	struct gps_dl_hw_dma_status_struct dma_status;
	struct gps_dl_hw_usrt_status_struct usrt_status;
	enum gps_dl_hal_dma_ch_index dma_ch;
	bool last_rw_log_on;
	unsigned int tick0, tick1;

	tick0 = gps_dl_tick_get();

	if (gps_dl_show_reg_wait_log())
		GDL_LOGXD(link_id, "timeout = %d", timeout_usec);

	if (link_id == GPS_DATA_LINK_ID0)
		dma_ch = GPS_DL_DMA_LINK0_D2A;
	else if (link_id == GPS_DATA_LINK_ID1)
		dma_ch = GPS_DL_DMA_LINK1_D2A;
	else
		return GDL_FAIL;

	while (1) {
		gps_dl_hw_save_dma_status_struct(dma_ch, &dma_status);
		if (gps_dl_only_show_wait_done_log())
			last_rw_log_on = gps_dl_set_show_reg_rw_log(false);
		else
			gps_dl_hw_print_dma_status_struct(dma_ch, &dma_status);

		if (GDL_HW_EXTRACT_ENTRY(BGF_GPS_DMA_DMA1_START_STR, dma_status.intr_flag) &&
			GDL_HW_EXTRACT_ENTRY(BGF_GPS_DMA_DMA1_STATE_STATE, dma_status.state) == 0x01) {
			if (gps_dl_only_show_wait_done_log()) {
				gps_dl_set_show_reg_rw_log(last_rw_log_on);
				gps_dl_hw_print_dma_status_struct(dma_ch, &dma_status);
				gps_dl_hw_save_dma_status_struct(dma_ch, &dma_status);
			}

			/* DMA has stopped */
			gps_dl_hw_set_dma_stop(dma_ch);
			gps_dl_hw_save_dma_status_struct(dma_ch, &dma_status);
			gps_dl_hw_print_dma_status_struct(dma_ch, &dma_status);
			gps_dl_hal_event_send(GPS_DL_HAL_EVT_D2A_RX_DMA_DONE, link_id);
			break;
		}

		gps_dl_hw_save_usrt_status_struct(link_id, &usrt_status);
		if (gps_dl_only_show_wait_done_log())
			last_rw_log_on = gps_dl_set_show_reg_rw_log(false);
		else
			gps_dl_hw_print_usrt_status_struct(link_id, &usrt_status);

		if (GDL_HW_EXTRACT_ENTRY(GPS_USRT_APB_APB_STA_REGE, usrt_status.state) ||
			GDL_HW_EXTRACT_ENTRY(GPS_USRT_APB_APB_STA_NODAINTB, usrt_status.state)) {
			if (gps_dl_only_show_wait_done_log()) {
				gps_dl_set_show_reg_rw_log(last_rw_log_on);
				gps_dl_hw_print_usrt_status_struct(link_id, &usrt_status);
				gps_dl_hw_save_usrt_status_struct(link_id, &usrt_status);
			}

			gps_dl_hal_event_send(GPS_DL_HAL_EVT_D2A_RX_HAS_NODATA, link_id);
			break;
		}

		tick1 = gps_dl_tick_get();
		if (timeout_usec > GPS_DL_RW_NO_TIMEOUT &&
			gps_dl_tick_delta_to_usec(tick0, tick1) >= timeout_usec)
			return GDL_FAIL_TIMEOUT;

		gps_dl_wait_us(GDL_HW_STATUS_POLL_INTERVAL_USEC);
	}

	return GDL_OKAY;
}

void gps_dl_hal_poll_single_link(enum gps_dl_link_id_enum link_id,
	unsigned int evt_in, unsigned int *p_evt_out)
{
	unsigned int evt_out = 0;
	struct gps_dl_hw_link_status_struct hw_status;

	gps_dl_hw_get_link_status(link_id, &hw_status);

	if (evt_in & (1UL << GPS_DL_POLL_TX_DMA_DONE)) {
		if (hw_status.tx_dma_done) {
			evt_out |= (1UL << GPS_DL_POLL_TX_DMA_DONE);
			gps_dl_isr_a2d_tx_dma_done(link_id);
		}
	}

	if (evt_in & (1UL << GPS_DL_POLL_RX_DMA_DONE)) {
		if (hw_status.rx_dma_done) {
			evt_out |= (1UL << GPS_DL_POLL_RX_DMA_DONE);
			gps_dl_isr_d2a_rx_dma_done(link_id);
		}
	}

	if (evt_in & (1UL << GPS_DL_POLL_USRT_HAS_DATA)) {
		if (hw_status.usrt_has_data) {
			evt_out |= (1UL << GPS_DL_POLL_USRT_HAS_DATA);
			gps_dl_isr_usrt_has_data(link_id);
		}
	}

	if (evt_in & (1UL << GPS_DL_POLL_USRT_HAS_NODATA)) {
		if (hw_status.usrt_has_nodata) {
			evt_out |= (1UL << GPS_DL_POLL_USRT_HAS_NODATA);
			gps_dl_isr_usrt_has_nodata(link_id);
		}
	}

	*p_evt_out = evt_out;
}

enum GDL_RET_STATUS gps_dl_hal_poll_event(
	unsigned int L1_evt_in, unsigned int L5_evt_in,
	unsigned int *pL1_evt_out, unsigned int *pL5_evt_out, unsigned int timeout_usec)
{
	unsigned int L1_evt_out = 0;
	unsigned int L5_evt_out = 0;
	enum GDL_RET_STATUS ret_val = GDL_OKAY;
	unsigned int tick0, tick1;
	int take_usec;

	if (L1_evt_in == 0 && L5_evt_in == 0) {
		*pL1_evt_out = 0;
		*pL5_evt_out = 0;
		return GDL_OKAY;
	}

	tick0 = gps_dl_tick_get();
	while (1) {
		if (L1_evt_in)
			gps_dl_hal_poll_single_link(GPS_DATA_LINK_ID0, L1_evt_in, &L1_evt_out);

		if (L5_evt_in)
			gps_dl_hal_poll_single_link(GPS_DATA_LINK_ID1, L5_evt_in, &L5_evt_out);

		tick1 = gps_dl_tick_get();
		take_usec = gps_dl_tick_delta_to_usec(tick0, tick1);

		if (L1_evt_out || L5_evt_out)
			break;

		GDL_LOGD("tick0 = %d, tick1 = %d, usec = %d/%d",
			tick0, tick1, take_usec, timeout_usec);

		if (take_usec >= timeout_usec) {
			ret_val = GDL_FAIL_TIMEOUT;
			break;
		}

		gps_dl_wait_us(GDL_HW_STATUS_POLL_INTERVAL_USEC);
	}

	GDL_LOGD("ret = %d, L1 = 0x%x, L5 = 0x%x, usec = %d/%d",
			ret_val, L1_evt_out, L5_evt_out, take_usec, timeout_usec);

	if (ret_val != GDL_OKAY)
		return ret_val;

	/* TODO: read one more time? */
	*pL1_evt_out = L1_evt_out;
	*pL5_evt_out = L5_evt_out;
	return GDL_OKAY;
}

int gps_dl_hal_usrt_direct_write(enum gps_dl_link_id_enum link_id,
	unsigned char *buf, unsigned int len)
{
	unsigned int reg_val;
	unsigned int reg_addr;
	int i, j;

	if (link_id == GPS_DATA_LINK_ID0)
		reg_addr = GPS_USRT_APB_GPS_APB_DATA_ADDR;
	else if (link_id == GPS_DATA_LINK_ID1)
		reg_addr = GPS_L5_USRT_APB_GPS_APB_DATA_ADDR;
	else
		return -1;

	if (gps_dl_is_1byte_mode()) {
		for (i = 0; i < len; i++)
			gps_dl_bus_write_no_rb(GPS_DL_GPS_BUS, reg_addr, buf[i]);
	} else {
		for (i = 0; i < len;) {
			reg_val = (unsigned int) buf[i++];

			for (j = 1; j < 4 && i < len; j++, i++)
				reg_val |= (((unsigned int) buf[i]) << (j * 8));

			gps_dl_bus_write_no_rb(GPS_DL_GPS_BUS, reg_addr, reg_val);
		}
	}

	return 0;
}

int gps_dl_hal_usrt_direct_read(enum gps_dl_link_id_enum link_id,
	unsigned char *buf, unsigned int len)
{
	unsigned int reg_val;
	unsigned int reg_addr;
	struct gps_dl_hw_link_status_struct hw_status;
	int i;

	if (link_id == GPS_DATA_LINK_ID0)
		reg_addr = GPS_USRT_APB_GPS_APB_DATA_ADDR;
	else if (link_id == GPS_DATA_LINK_ID1)
		reg_addr = GPS_L5_USRT_APB_GPS_APB_DATA_ADDR;
	else
		return -1;

	/* GPS_USRT_APB_APB_STA_TXINTB_SHFT */
	/* APB_STA[7:0]: 0x22000030 -> 0x20000009 -> 0x22000019 -> 0x22000030 */
	do {
		gps_dl_hw_get_link_status(link_id, &hw_status);
	} while (!hw_status.usrt_has_data);

	for (i = 0; i < len; ) {
		reg_val = GDL_HW_RD_GPS_REG(reg_addr);

		if (gps_dl_is_1byte_mode())
			buf[i++] = (unsigned char)reg_val;
		else {
			buf[i++] = (unsigned char)(reg_val >> 0);
			buf[i++] = (unsigned char)(reg_val >> 8);
			buf[i++] = (unsigned char)(reg_val >> 16);
			buf[i++] = (unsigned char)(reg_val >> 24);
		}

		gps_dl_hw_get_link_status(link_id, &hw_status);
		if (!hw_status.usrt_has_data) /* no need: hw_status.usrt_has_nodata */
			break;
	}

	GDL_LOGXD(link_id, "read len = %d", i);
	return i;
}

