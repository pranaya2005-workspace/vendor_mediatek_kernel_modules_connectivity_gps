/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */
#ifndef __CONN_RF_SPI_MST_REG_REGS_H__
#define __CONN_RF_SPI_MST_REG_REGS_H__

#define CONN_RF_SPI_MST_REG_BASE                               0x18004000

#define CONN_RF_SPI_MST_ADDR_SPI_STA_ADDR                      (CONN_RF_SPI_MST_REG_BASE + 0x0000)
#define CONN_RF_SPI_MST_ADDR_FM_CTRL_ADDR                      (CONN_RF_SPI_MST_REG_BASE + 0x000C)
#define CONN_RF_SPI_MST_ADDR_FM_CTRL_ADDR                      (CONN_RF_SPI_MST_REG_BASE + 0x000C)
#define CONN_RF_SPI_MST_ADDR_SPI_FM_ADDR_ADDR                  (CONN_RF_SPI_MST_REG_BASE + 0x0030)
#define CONN_RF_SPI_MST_ADDR_SPI_FM_WDAT_ADDR                  (CONN_RF_SPI_MST_REG_BASE + 0x0034)
#define CONN_RF_SPI_MST_ADDR_SPI_FM_RDAT_ADDR                  (CONN_RF_SPI_MST_REG_BASE + 0x0038)
#define CONN_RF_SPI_MST_ADDR_SPI_GPS_GPS_ADDR_ADDR             (CONN_RF_SPI_MST_REG_BASE + 0x0210)
#define CONN_RF_SPI_MST_ADDR_SPI_GPS_GPS_WDAT_ADDR             (CONN_RF_SPI_MST_REG_BASE + 0x0214)
#define CONN_RF_SPI_MST_ADDR_SPI_GPS_GPS_RDAT_ADDR             (CONN_RF_SPI_MST_REG_BASE + 0x0218)


#define CONN_RF_SPI_MST_REG_SPI_STA_FM_BUSY_ADDR               CONN_RF_SPI_MST_ADDR_SPI_STA_ADDR
#define CONN_RF_SPI_MST_REG_SPI_STA_FM_BUSY_MASK               0x00000008
#define CONN_RF_SPI_MST_REG_SPI_STA_FM_BUSY_SHFT               3

#define CONN_RF_SPI_MST_REG_FM_CTRL_FM_RD_EXT_EN_ADDR          CONN_RF_SPI_MST_ADDR_FM_CTRL_ADDR
#define CONN_RF_SPI_MST_REG_FM_CTRL_FM_RD_EXT_EN_MASK          0x00008000
#define CONN_RF_SPI_MST_REG_FM_CTRL_FM_RD_EXT_EN_SHFT          15
#define CONN_RF_SPI_MST_REG_FM_CTRL_FM_RD_EXT_CNT_ADDR         CONN_RF_SPI_MST_ADDR_FM_CTRL_ADDR
#define CONN_RF_SPI_MST_REG_FM_CTRL_FM_RD_EXT_CNT_MASK         0x000000FF
#define CONN_RF_SPI_MST_REG_FM_CTRL_FM_RD_EXT_CNT_SHFT         0

#endif /* __CONN_RF_SPI_MST_REG_REGS_H__ */

