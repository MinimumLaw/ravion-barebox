/* SPDX-License-Identifier: GPL-2.0 */
/**
 * gadget.h - DesignWare USB3 DRD Gadget Header
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com
 *
 * Authors: Felipe Balbi <balbi@ti.com>,
 *	    Sebastian Andrzej Siewior <bigeasy@linutronix.de>
 *
 * Taken from Linux Kernel v3.19-rc1 (drivers/usb/dwc3/gadget.h) and ported
 * to uboot.
 *
 * commit 7a60855972 : usb: dwc3: gadget: fix set_halt() bug with pending
		       transfers
 *
 */

#ifndef __DRIVERS_USB_DWC3_GADGET_H
#define __DRIVERS_USB_DWC3_GADGET_H

#include <usb/gadget.h>
#include <linux/list.h>
#include "io.h"

struct dwc3;
#define to_dwc3_ep(ep)		(container_of(ep, struct dwc3_ep, endpoint))
#define gadget_to_dwc(g)	(container_of(g, struct dwc3, gadget))

/* DEPCFG parameter 1 */
#define DWC3_DEPCFG_INT_NUM(n)		((n) << 0)
#define DWC3_DEPCFG_XFER_COMPLETE_EN	(1 << 8)
#define DWC3_DEPCFG_XFER_IN_PROGRESS_EN (1 << 9)
#define DWC3_DEPCFG_XFER_NOT_READY_EN	(1 << 10)
#define DWC3_DEPCFG_FIFO_ERROR_EN	(1 << 11)
#define DWC3_DEPCFG_STREAM_EVENT_EN	(1 << 13)
#define DWC3_DEPCFG_BINTERVAL_M1(n)	((n) << 16)
#define DWC3_DEPCFG_STREAM_CAPABLE	(1 << 24)
#define DWC3_DEPCFG_EP_NUMBER(n)	((n) << 25)
#define DWC3_DEPCFG_BULK_BASED		(1 << 30)
#define DWC3_DEPCFG_FIFO_BASED		(1 << 31)

/* DEPCFG parameter 0 */
#define DWC3_DEPCFG_EP_TYPE(n)		((n) << 1)
#define DWC3_DEPCFG_MAX_PACKET_SIZE(n)	((n) << 3)
#define DWC3_DEPCFG_FIFO_NUMBER(n)	((n) << 17)
#define DWC3_DEPCFG_BURST_SIZE(n)	((n) << 22)
#define DWC3_DEPCFG_DATA_SEQ_NUM(n)	((n) << 26)
/* This applies for core versions earlier than 1.94a */
#define DWC3_DEPCFG_IGN_SEQ_NUM	(1 << 31)
/* These apply for core versions 1.94a and later */
#define DWC3_DEPCFG_ACTION_INIT	(0 << 30)
#define DWC3_DEPCFG_ACTION_RESTORE	(1 << 30)
#define DWC3_DEPCFG_ACTION_MODIFY	(2 << 30)

/* DEPXFERCFG parameter 0 */
#define DWC3_DEPXFERCFG_NUM_XFER_RES(n)	((n) & 0xffff)

/* -------------------------------------------------------------------------- */

#define to_dwc3_request(r)	(container_of(r, struct dwc3_request, request))

static inline struct dwc3_request *next_request(struct list_head *list)
{
	if (list_empty(list))
		return NULL;

	return list_first_entry(list, struct dwc3_request, list);
}

/**
 * dwc3_gadget_move_started_request - move @req to the started_list
 * @req: the request to be moved
 *
 * Caller should take care of locking. This function will move @req from its
 * current list to the endpoint's started_list.
 */
static inline void dwc3_gadget_move_started_request(struct dwc3_request *req)
{
	struct dwc3_ep *dep = req->dep;

	req->status = DWC3_REQUEST_STATUS_STARTED;
	list_move_tail(&req->list, &dep->started_list);
}

/**
 * dwc3_gadget_move_cancelled_request - move @req to the cancelled_list
 * @req: the request to be moved
 *
 * Caller should take care of locking. This function will move @req from its
 * current list to the endpoint's cancelled_list.
 */
static inline void dwc3_gadget_move_cancelled_request(struct dwc3_request *req)
{
	struct dwc3_ep *dep = req->dep;

	req->status = DWC3_REQUEST_STATUS_CANCELLED;
	list_move_tail(&req->list, &dep->cancelled_list);
}

void dwc3_gadget_giveback(struct dwc3_ep *dep, struct dwc3_request *req,
		int status);

void dwc3_ep0_interrupt(struct dwc3 *dwc,
		const struct dwc3_event_depevt *event);
void dwc3_ep0_out_start(struct dwc3 *dwc);
int __dwc3_gadget_ep0_set_halt(struct usb_ep *ep, int value);
int dwc3_gadget_ep0_set_halt(struct usb_ep *ep, int value);
int dwc3_gadget_ep0_queue(struct usb_ep *ep, struct usb_request *request);
int __dwc3_gadget_ep_set_halt(struct dwc3_ep *dep, int value, int protocol);
void dwc3_gadget_handle_interrupt(struct dwc3 *dwc);

/**
 * dwc3_gadget_ep_get_transfer_index - Gets transfer index from HW
 * @dep: dwc3 endpoint
 *
 * Caller should take care of locking. Returns the transfer resource
 * index for a given endpoint.
 */
static inline void dwc3_gadget_ep_get_transfer_index(struct dwc3_ep *dep)
{
	u32 res_id;

	res_id = dwc3_readl(dep->regs, DWC3_DEPCMD);
	dep->resource_index = DWC3_DEPCMD_GET_RSC_IDX(res_id);
}

#endif /* __DRIVERS_USB_DWC3_GADGET_H */