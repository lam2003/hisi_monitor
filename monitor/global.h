#pragma once

//stl
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//system
#include <unistd.h>
//logger
#include <elog.h>
//hisi sdk
#include <hi_common.h>
#include <hi_comm_sys.h>
#include <hi_comm_vb.h>
#include <hi_comm_isp.h>
#include <hi_comm_vi.h>
#include <hi_comm_vo.h>
#include <hi_comm_venc.h>
#include <hi_comm_vpss.h>
#include <hi_comm_region.h>
#include <hi_comm_adec.h>
#include <hi_comm_aenc.h>
#include <hi_comm_ai.h>
#include <hi_comm_ao.h>
#include <hi_comm_aio.h>
#include <hi_defines.h>
#include <mpi_sys.h>
#include <mpi_vb.h>
#include <mpi_vi.h>
#include <mpi_vo.h>
#include <mpi_venc.h>
#include <mpi_vpss.h>
#include <mpi_region.h>
#include <mpi_adec.h>
#include <mpi_aenc.h>
#include <mpi_ai.h>
#include <mpi_ao.h>
#include <mpi_isp.h>
#include <mpi_ae.h>
#include <mpi_awb.h>
#include <mpi_af.h>
#include <hi_vreg.h>
#include <hi_sns_ctrl.h>

#define NVR_CHECK(condition) \
    while (!(condition))     \
    log_e("check %s failed", #condition)

// arm-hisiv500-linux-g++²»Ö§³Ö
// template<typename T>
// T GLOBAL;