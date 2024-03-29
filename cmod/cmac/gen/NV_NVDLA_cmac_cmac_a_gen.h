// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: NV_NVDLA_cmac_cmac_a_gen.h

#include "log.h"
#include "NV_NVDLA_cmac.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

USING_SCSIM_NAMESPACE(cmod)
USING_SCSIM_NAMESPACE(clib)

inline void
NV_NVDLA_cmac::csb2cmac_a_req_b_transport (int ID, NV_MSDEC_csb2xx_16m_secure_be_lvl_t* payload, sc_time& delay) {
    uint32_t addr;
    uint32_t data;
    uint8_t write;
    uint8_t nposted;
    uint8_t error_id=0;
    bool    register_access_result;
    if (true == is_there_ongoing_csb2cmac_a_response_) {
#pragma CTC SKIP
        FAIL(("NVDLA NV_NVDLA_cmac::csb2cmac_a_b_transport: there is onging CSB response."));
#pragma CTC ENDSKIP
    } else {
        is_there_ongoing_csb2cmac_a_response_ = true;
    }
    // Extract CSB request information from payload
    if (NULL != payload) {
        addr = payload->pd.csb2xx_16m_secure_be_lvl.addr;
        data = payload->pd.csb2xx_16m_secure_be_lvl.wdat;
        write = payload->pd.csb2xx_16m_secure_be_lvl.write;
        nposted = payload->pd.csb2xx_16m_secure_be_lvl.nposted;
    } else {
#pragma CTC SKIP
        FAIL(("NVDLA NV_NVDLA_cmac::csb2cmac_a_b_transport: payload pointer shall not be NULL"));
#pragma CTC ENDSKIP
    }
    // Accessing registers                           // 从csb接受信号，并把信号存到CMAC的register里面
    register_access_result = CmacAAccessRegister (addr, data, 0!=write);
    if (false == register_access_result) {
        error_id = 1;                                // 判断是否成功将信息存入register
    }
    // Read and nposted write need to send response data
    if ( (0==write) || (0!=nposted) ) {
        // Read     or  is non-posted
        CmacASendCsbResponse(write, data, error_id); // 不管csb写入register成功与否，给csb返回response信息
    }
    is_there_ongoing_csb2cmac_a_response_ = false;   // flag，用来标记这个模块已经用完被释放
}

inline void
NV_NVDLA_cmac::CmacASendCsbResponse(uint8_t type, uint32_t data, uint8_t error_id) {
    nvdla_xx2csb_resp_t payload;
    if (0==type) {
        // Read return data
        payload.pd.xx2csb_rd_erpt.error   = error_id;
        payload.pd.xx2csb_rd_erpt.rdat    = data;
		payload.tag =  XX2CSB_RESP_TAG_READ;
    } else {
        // Write return data
        payload.pd.xx2csb_wr_erpt.error   = error_id;
        payload.pd.xx2csb_wr_erpt.rdat    = 0;
		payload.tag =  XX2CSB_RESP_TAG_WRITE;
    }
    NV_NVDLA_cmac_base::cmac_a2csb_resp_b_transport(&payload, b_transport_delay_);
}
