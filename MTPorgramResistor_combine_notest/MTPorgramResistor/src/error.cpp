#include "error.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "visa.h"



int mt_translate_sys_error(int sys_errno) {

	if ((sys_errno <= 0) && (sys_errno > -5000)) {
		return sys_errno;  /* If < 0 then it's already a mt error. */
	}
	switch (sys_errno) {
		//VISA  Error codes(-5001,-6000)
	case VI_ERROR_SYSTEM_ERROR:				return MT__EVI_SYSTEM_ERROR;
	case VI_ERROR_INV_OBJECT:				return MT__EVI_INV_OBJECT;
	case VI_ERROR_RSRC_LOCKED:				return MT__EVI_RSRC_LOCKED;
	case VI_ERROR_INV_EXPR:                 return MT__EVI_INV_EXPR;
	case VI_ERROR_RSRC_NFOUND:				return MT__EVI_RSRC_NFOUND;
	case VI_ERROR_INV_RSRC_NAME:			return MT__EVI_INV_RSRC_NAME;
	case VI_ERROR_INV_ACC_MODE:				return MT__EVI_INV_ACC_MODE;
	case VI_ERROR_TMO:                 return MT__EVI_TMO;
	case VI_ERROR_CLOSING_FAILED:      return MT__EVI_CLOSING_FAILED;
	case VI_ERROR_INV_DEGREE:          return MT__EVI_RSRC_LOCKED;
	case VI_ERROR_INV_JOB_ID:          return MT__EVI_INV_JOB_ID;
	case VI_ERROR_NSUP_ATTR:           return MT__EVI_NSUP_ATTR;
	case VI_ERROR_NSUP_ATTR_STATE:     return MT__EVI_NSUP_ATTR_STATE;
	case VI_ERROR_ATTR_READONLY:       return MT__EVI_ATTR_READONLY;
	case VI_ERROR_INV_LOCK_TYPE:       return MT__EVI_INV_LOCK_TYPE;
	case VI_ERROR_INV_ACCESS_KEY:      return MT__EVI_INV_ACCESS_KEY;
	case VI_ERROR_INV_EVENT:           return MT__EVI_INV_EVENT;
	case VI_ERROR_INV_MECH:            return MT__EVI_INV_MECH;
	case VI_ERROR_HNDLR_NINSTALLED:    return MT__EVI_HNDLR_NINSTALLED;
	case VI_ERROR_INV_HNDLR_REF:       return MT__EVI_INV_HNDLR_REF;
	case VI_ERROR_INV_CONTEXT:         return MT__EVI_INV_CONTEXT;
	case VI_ERROR_QUEUE_OVERFLOW:      return MT__EVI_QUEUE_OVERFLOW;
	case VI_ERROR_NENABLED:            return MT__EVI_NENABLED;
	case VI_ERROR_ABORT:               return MT__EVI_ABORT;
	case VI_ERROR_RAW_WR_PROT_VIOL:    return MT__EVI_RAW_WR_PROT_VIOL;
	case VI_ERROR_RAW_RD_PROT_VIOL:    return MT__EVI_RAW_RD_PROT_VIOL;
	case VI_ERROR_OUTP_PROT_VIOL:      return MT__EVI_OUTP_PROT_VIOL;
	case VI_ERROR_INP_PROT_VIOL:       return MT__EVI_INP_PROT_VIOL;
	case VI_ERROR_BERR:                return MT__EVI_BERR;
	case VI_ERROR_IN_PROGRESS:         return MT__EVI_IN_PROGRESS;
	case VI_ERROR_INV_SETUP:           return MT__EVI_INV_SETUP;
	case VI_ERROR_QUEUE_ERROR:         return MT__EVI_QUEUE_ERROR;
	case VI_ERROR_ALLOC:               return MT__EVI_ALLOC;
	case VI_ERROR_INV_MASK:            return MT__EVI_INV_MASK;
	case VI_ERROR_IO:                  return MT__EVI_IO;
	case VI_ERROR_INV_FMT:             return MT__EVI_INV_FMT;
	case VI_ERROR_NSUP_FMT:            return MT__EVI_NSUP_FMT;
	case VI_ERROR_LINE_IN_USE:         return MT__EVI_LINE_IN_USE;
	case VI_ERROR_LINE_NRESERVED:      return MT__EVI_LINE_NRESERVED;
	case VI_ERROR_NSUP_MODE:           return MT__EVI_NSUP_MODE;
	case VI_ERROR_SRQ_NOCCURRED:       return MT__EVI_SRQ_NOCCURRED;
	case VI_ERROR_INV_SPACE:           return MT__EVI_INV_SPACE;
	case VI_ERROR_INV_OFFSET:          return MT__EVI_INV_OFFSET;
	case VI_ERROR_INV_WIDTH:          return MT__EVI_INV_WIDTH;
	case VI_ERROR_NSUP_OFFSET:        return MT__EVI_NSUP_OFFSET;
	case VI_ERROR_NSUP_VAR_WIDTH:     return MT__EVI_NSUP_VAR_WIDTH;
	case VI_ERROR_WINDOW_NMAPPED:     return MT__EVI_WINDOW_NMAPPED;
	case VI_ERROR_RESP_PENDING:       return MT__EVI_RESP_PENDING;
	case VI_ERROR_NLISTENERS:         return MT__EVI_NLISTENERS;
	case VI_ERROR_NCIC:               return MT__EVI_NCIC;
	case VI_ERROR_NSYS_CNTLR:         return MT__EVI_NSYS_CNTLR;
	case VI_ERROR_NSUP_OPER:          return MT__EVI_NSUP_OPER;
	case VI_ERROR_INTR_PENDING:       return MT__EVI_INTR_PENDING;
	case VI_ERROR_ASRL_PARITY:        return MT__EVI_ASRL_PARITY;
	case VI_ERROR_ASRL_FRAMING:       return MT__EVI_ASRL_FRAMING;
	case VI_ERROR_ASRL_OVERRUN:       return MT__EVI_ASRL_OVERRUN;
	case VI_ERROR_TRIG_NMAPPED:       return MT__EVI_TRIG_NMAPPED;
	case VI_ERROR_NSUP_ALIGN_OFFSET:  return MT__EVI_NSUP_ALIGN_OFFSET;
	case VI_ERROR_USER_BUF:           return MT__EVI_USER_BUF;
	case VI_ERROR_RSRC_BUSY:          return MT__EVI_RSRC_BUSY;
	case VI_ERROR_NSUP_WIDTH:         return MT__EVI_NSUP_WIDTH;
	case VI_ERROR_INV_PARAMETER:      return MT__EVI_INV_PARAMETER;
	case VI_ERROR_INV_PROT:           return MT__EVI_INV_PROT;
	case VI_ERROR_INV_SIZE:           return MT__EVI_INV_SIZE;
	case VI_ERROR_WINDOW_MAPPED:      return MT__EVI_WINDOW_MAPPED;
	case VI_ERROR_NIMPL_OPER:         return MT__EVI_NIMPL_OPER;
	case VI_ERROR_INV_LENGTH:         return MT__EVI_INV_LENGTH;
	case VI_ERROR_INV_MODE:           return MT__EVI_INV_MODE;
	case VI_ERROR_SESN_NLOCKED:       return MT__EVI_SESN_NLOCKED;
	case VI_ERROR_MEM_NSHARED:        return MT__EVI_MEM_NSHARED;
	case VI_ERROR_LIBRARY_NFOUND:     return MT__EVI_LIBRARY_NFOUND;
	case VI_ERROR_NSUP_INTR:          return MT__EVI_NSUP_INTR;
	case VI_ERROR_INV_LINE:           return MT__EVI_INV_LINE;
	case VI_ERROR_FILE_ACCESS:        return MT__EVI_FILE_ACCESS;
	case VI_ERROR_FILE_IO:            return MT__EVI_FILE_IO;
	case VI_ERROR_NSUP_LINE:          return MT__EVI_NSUP_LINE;
	case VI_ERROR_NSUP_MECH:          return MT__EVI_NSUP_MECH;
	case VI_ERROR_INTF_NUM_NCONFIG:   return MT__EVI_INTF_NUM_NCONFIG;
	case VI_ERROR_CONN_LOST:          return MT__EVI_CONN_LOST;
	case VI_ERROR_MACHINE_NAVAIL:     return MT__EVI_MACHINE_NAVAIL;
	case VI_ERROR_NPERMISSION:        return MT__EVI_NPERMISSION;
	//  //MT525  Error codes(-4001,-5000)
	//	//MTPorgramResistor Error codes(-3001,-4000)
	default:
		return MT__UNKNOWN;
	}
}


