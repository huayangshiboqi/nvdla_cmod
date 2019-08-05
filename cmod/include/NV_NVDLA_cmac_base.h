// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: NV_NVDLA_cmac_base.h

#ifndef _NV_NVDLA_CMAC_BASE_H_
#define _NV_NVDLA_CMAC_BASE_H_

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "NV_MSDEC_csb2xx_16m_secure_be_lvl_iface.h"
#include "nvdla_xx2csb_resp_iface.h"

#include "nvdla_mac2accu_data_if_iface.h"
#include "nvdla_sc2mac_data_if_iface.h"
#include "nvdla_sc2mac_weight_if_iface.h"
#include "scsim_common.h"
#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/multi_passthrough_target_socket.h>

SCSIM_NAMESPACE_START(cmod)

//ports。 TLM2.0规则中的Initiator + target 的进程间通信方式
//而关于Systemc Module的写法，http://blog.chinaunix.net/uid-8867796-id-358790.html
// Base SystemC class for module NV_NVDLA_cmac
class NV_NVDLA_cmac_base : public sc_module             //从类sc_module中，继承某些属性。和SC_MODULE NV_NVDLA_cmac_base差不多
{
    public:

    // Constructor
    NV_NVDLA_cmac_base(const sc_module_name name);

    // Target Socket (unrecognized protocol: NV_MSDEC_csb2xx_16m_secure_be_lvl_t): csb2cmac_req
    tlm_utils::multi_passthrough_target_socket<NV_NVDLA_cmac_base, 32, tlm::tlm_base_protocol_types> csb2cmac_req;
    virtual void csb2cmac_a_req_b_transport(int ID, tlm::tlm_generic_payload& bp, sc_time& delay);
    virtual void csb2cmac_a_req_b_transport(int ID, NV_MSDEC_csb2xx_16m_secure_be_lvl_t* payload, sc_time& delay) = 0;

    // Target Socket (unrecognized protocol: nvdla_sc2mac_data_if_t): sc2mac_dat
    tlm_utils::multi_passthrough_target_socket<NV_NVDLA_cmac_base, 32, tlm::tlm_base_protocol_types> sc2mac_dat;
    virtual void sc2mac_dat_b_transport(int ID, tlm::tlm_generic_payload& bp, sc_time& delay);
    virtual void sc2mac_dat_b_transport(int ID, nvdla_sc2mac_data_if_t* payload, sc_time& delay) = 0;

    // Target Socket (unrecognized protocol: nvdla_sc2mac_weight_if_t): sc2mac_wt
    tlm_utils::multi_passthrough_target_socket<NV_NVDLA_cmac_base, 32, tlm::tlm_base_protocol_types> sc2mac_wt;
    virtual void sc2mac_wt_b_transport(int ID, tlm::tlm_generic_payload& bp, sc_time& delay);
    virtual void sc2mac_wt_b_transport(int ID, nvdla_sc2mac_weight_if_t* payload, sc_time& delay) = 0;

    // Initiator Socket (unrecognized protocol: nvdla_xx2csb_resp_t): cmac2csb_resp
    tlm::tlm_generic_payload cmac2csb_resp_bp;
    nvdla_xx2csb_resp_t cmac2csb_resp_payload;
    tlm_utils::multi_passthrough_initiator_socket<NV_NVDLA_cmac_base, 32, tlm::tlm_base_protocol_types> cmac2csb_resp;
    virtual void cmac_a2csb_resp_b_transport(nvdla_xx2csb_resp_t* payload, sc_time& delay);

    // Initiator Socket (unrecognized protocol: nvdla_mac2accu_data_if_t): mac2accu
    tlm::tlm_generic_payload mac2accu_bp;                                                  //tlm_generic_payload是default transaction type for the socket classes
    nvdla_mac2accu_data_if_t mac2accu_payload;
    tlm_utils::multi_passthrough_initiator_socket<NV_NVDLA_cmac_base, 32, tlm::tlm_base_protocol_types> mac2accu;
    virtual void mac2accu_b_transport(nvdla_mac2accu_data_if_t* payload, sc_time& delay);  //virtual关键字规定一种虚拟函数。目的是在派生类中，可以重写虚拟函数来实现对基类虚拟函数的覆盖（覆盖和函数重载不一样）
                                                                                           //因为该基类会被很多类继承。在派生类中，你可以规定自己的函数

    // Destructor
    virtual ~NV_NVDLA_cmac_base() {}

};

// Constructor for base SystemC class for module NV_NVDLA_cmac
inline NV_NVDLA_cmac_base::NV_NVDLA_cmac_base(const sc_module_name name)
    : sc_module(name),
    csb2cmac_req("csb2cmac_req"),
    sc2mac_dat("sc2mac_dat"),
    sc2mac_wt("sc2mac_wt"),
    cmac2csb_resp_bp(),
    cmac2csb_resp("cmac2csb_resp"),
    mac2accu_bp(),
    mac2accu("mac2accu")
{
    // Target Socket (unrecognized protocol: NV_MSDEC_csb2xx_16m_secure_be_lvl_t): csb2cmac_req
    this->csb2cmac_req.register_b_transport(this, &NV_NVDLA_cmac_base::csb2cmac_a_req_b_transport);

    // Target Socket (unrecognized protocol: nvdla_sc2mac_data_if_t): sc2mac_dat
    this->sc2mac_dat.register_b_transport(this, &NV_NVDLA_cmac_base::sc2mac_dat_b_transport);

    // Target Socket (unrecognized protocol: nvdla_sc2mac_weight_if_t): sc2mac_wt
    this->sc2mac_wt.register_b_transport(this, &NV_NVDLA_cmac_base::sc2mac_wt_b_transport);

}

inline void
NV_NVDLA_cmac_base::csb2cmac_a_req_b_transport(int ID, tlm::tlm_generic_payload& bp, sc_time& delay)
{
    NV_MSDEC_csb2xx_16m_secure_be_lvl_t* payload = (NV_MSDEC_csb2xx_16m_secure_be_lvl_t*) bp.get_data_ptr();
    csb2cmac_a_req_b_transport(ID, payload, delay);
}

inline void
NV_NVDLA_cmac_base::sc2mac_dat_b_transport(int ID, tlm::tlm_generic_payload& bp, sc_time& delay)
{
    nvdla_sc2mac_data_if_t* payload = (nvdla_sc2mac_data_if_t*) bp.get_data_ptr();
    sc2mac_dat_b_transport(ID, payload, delay);
}

inline void
NV_NVDLA_cmac_base::sc2mac_wt_b_transport(int ID, tlm::tlm_generic_payload& bp, sc_time& delay)
{
    nvdla_sc2mac_weight_if_t* payload = (nvdla_sc2mac_weight_if_t*) bp.get_data_ptr();
    sc2mac_wt_b_transport(ID, payload, delay);
}

inline void
NV_NVDLA_cmac_base::cmac_a2csb_resp_b_transport(nvdla_xx2csb_resp_t* payload, sc_time& delay)
{
    cmac2csb_resp_bp.set_data_ptr((unsigned char*) payload);
    for (uint8_t socket_id=0; socket_id < cmac2csb_resp.size(); socket_id++) {
        cmac2csb_resp[socket_id]->b_transport(cmac2csb_resp_bp, delay);
    }
}

inline void
NV_NVDLA_cmac_base::mac2accu_b_transport(nvdla_mac2accu_data_if_t* payload, sc_time& delay)
{
    mac2accu_bp.set_data_ptr((unsigned char*) payload);
    for (uint8_t socket_id=0; socket_id < mac2accu.size(); socket_id++) {
        mac2accu[socket_id]->b_transport(mac2accu_bp, delay);
    }
}

SCSIM_NAMESPACE_END()

#endif
