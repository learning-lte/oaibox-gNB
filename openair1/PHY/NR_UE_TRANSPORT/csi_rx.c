/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/***********************************************************************
*
* FILENAME    :  csi_rx.c
*
* MODULE      :
*
* DESCRIPTION :  function to receive the channel state information
*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nr_transport_proto_ue.h"
#include "PHY/phy_extern_nr_ue.h"
#include "common/utils/nr/nr_common.h"
#include "PHY/NR_TRANSPORT/nr_transport_proto.h"
#include "PHY/NR_UE_ESTIMATION/filt16a_32.h"


//#define NR_CSIRS_DEBUG

bool is_csi_rs_in_symbol(fapi_nr_dl_config_csirs_pdu_rel15_t csirs_config_pdu, int symbol) {

  bool ret = false;

  // 38.211-Table 7.4.1.5.3-1: CSI-RS locations within a slot
  switch(csirs_config_pdu.row){
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 9:
      if(symbol == csirs_config_pdu.symb_l0) {
        ret = true;
      }
      break;
    case 5:
    case 7:
    case 8:
    case 10:
    case 11:
    case 12:
      if(symbol == csirs_config_pdu.symb_l0 || symbol == (csirs_config_pdu.symb_l0+1) ) {
        ret = true;
      }
      break;
    case 13:
    case 14:
    case 16:
    case 17:
      if(symbol == csirs_config_pdu.symb_l0 || symbol == (csirs_config_pdu.symb_l0+1) ||
          symbol == csirs_config_pdu.symb_l1 || symbol == (csirs_config_pdu.symb_l1+1)) {
        ret = true;
      }
      break;
    case 15:
    case 18:
      if(symbol == csirs_config_pdu.symb_l0 || symbol == (csirs_config_pdu.symb_l0+1) || symbol == (csirs_config_pdu.symb_l0+2) ) {
        ret = true;
      }
      break;
    default:
      AssertFatal(0==1, "Row %d is not valid for CSI Table 7.4.1.5.3-1\n", csirs_config_pdu.row);
  }

  return ret;
}

int nr_get_csi_rs_signal(PHY_VARS_NR_UE *ue,
                         UE_nr_rxtx_proc_t *proc,
                         fapi_nr_dl_config_csirs_pdu_rel15_t *csirs_config_pdu,
                         nr_csi_rs_info_t *nr_csi_rs_info,
                         int32_t **csi_rs_received_signal) {

  int32_t **rxdataF  =  ue->common_vars.common_vars_rx_data_per_thread[proc->thread_id].rxdataF;
  NR_DL_FRAME_PARMS *frame_parms = &ue->frame_parms;

  for (int ant_rx = 0; ant_rx < frame_parms->nb_antennas_rx; ant_rx++) {
    memset(csi_rs_received_signal[ant_rx], 0, frame_parms->samples_per_frame_wCP*sizeof(int32_t));

    for (int rb = csirs_config_pdu->start_rb; rb < (csirs_config_pdu->start_rb+csirs_config_pdu->nr_of_rbs); rb++) {

      // for freq density 0.5 checks if even or odd RB
      if(csirs_config_pdu->freq_density <= 1 && csirs_config_pdu->freq_density != (rb % 2)) {
        continue;
      }

      for (int cdm_id = 0; cdm_id < nr_csi_rs_info->N_cdm_groups; cdm_id++) {
        for (int s = 0; s < nr_csi_rs_info->CDM_group_size; s++)  {

          // loop over frequency resource elements within a group
          for (int kp = 0; kp <= nr_csi_rs_info->kprime; kp++) {

            uint16_t k = (frame_parms->first_carrier_offset + (rb*NR_NB_SC_PER_RB)+nr_csi_rs_info->koverline[cdm_id] + kp) % frame_parms->ofdm_symbol_size;

            // loop over time resource elements within a group
            for (int lp = 0; lp <= nr_csi_rs_info->lprime; lp++) {
              uint16_t symb = lp + nr_csi_rs_info->loverline[cdm_id];
              uint64_t symbol_offset = symb*frame_parms->ofdm_symbol_size;
              int16_t *rx_signal = (int16_t*)&rxdataF[ant_rx][symbol_offset];
              int16_t *rx_csi_rs_signal = (int16_t*)&csi_rs_received_signal[ant_rx][symbol_offset];
              rx_csi_rs_signal[k<<1] = rx_signal[k<<1];
              rx_csi_rs_signal[(k<<1)+1] = rx_signal[(k<<1)+1];

#ifdef NR_CSIRS_DEBUG
              int dataF_offset = proc->nr_slot_rx*ue->frame_parms.samples_per_slot_wCP;
              uint16_t port_tx = s+nr_csi_rs_info->j[cdm_id]*nr_csi_rs_info->CDM_group_size;
              int16_t *tx_csi_rs_signal = (int16_t*)&nr_csi_rs_info->csi_rs_generated_signal[port_tx][symbol_offset+dataF_offset];
              LOG_I(NR_PHY, "l,k (%2d,%3d) |\tport_tx %d (%4d,%4d)\tant_rx %d (%4d,%4d)\n",
                    symb,
                    k,
                    port_tx+3000,
                    tx_csi_rs_signal[k<<1],
                    tx_csi_rs_signal[(k<<1)+1],
                    ant_rx,
                    rx_csi_rs_signal[k<<1],
                    rx_csi_rs_signal[(k<<1)+1]);
#endif
            }
          }
        }
      }
    }
  }

  return 0;
}

int nr_csi_rs_channel_estimation(PHY_VARS_NR_UE *ue,
                                 UE_nr_rxtx_proc_t *proc,
                                 fapi_nr_dl_config_csirs_pdu_rel15_t *csirs_config_pdu,
                                 nr_csi_rs_info_t *nr_csi_rs_info,
                                 int32_t **csi_rs_generated_signal,
                                 int32_t **csi_rs_received_signal,
                                 int32_t ***csi_rs_estimated_channel_freq,
                                 uint32_t *noise_power) {

  NR_DL_FRAME_PARMS *frame_parms = &ue->frame_parms;
  int dataF_offset = proc->nr_slot_rx*ue->frame_parms.samples_per_slot_wCP;

  for (int ant_rx = 0; ant_rx < frame_parms->nb_antennas_rx; ant_rx++) {

    /// LS channel estimation

    for(uint16_t port_tx = 0; port_tx<nr_csi_rs_info->N_ports; port_tx++) {
      memset(nr_csi_rs_info->csi_rs_ls_estimated_channel[ant_rx][port_tx], 0, frame_parms->samples_per_frame_wCP*sizeof(int32_t));
    }

    for (int rb = csirs_config_pdu->start_rb; rb < (csirs_config_pdu->start_rb+csirs_config_pdu->nr_of_rbs); rb++) {

      // for freq density 0.5 checks if even or odd RB
      if(csirs_config_pdu->freq_density <= 1 && csirs_config_pdu->freq_density != (rb % 2)) {
        continue;
      }

      for (int cdm_id = 0; cdm_id < nr_csi_rs_info->N_cdm_groups; cdm_id++) {
        for (int s = 0; s < nr_csi_rs_info->CDM_group_size; s++)  {

          uint16_t port_tx = s+nr_csi_rs_info->j[cdm_id]*nr_csi_rs_info->CDM_group_size;

          // loop over frequency resource elements within a group
          for (int kp = 0; kp <= nr_csi_rs_info->kprime; kp++) {

            uint16_t k = (frame_parms->first_carrier_offset + (rb*NR_NB_SC_PER_RB)+nr_csi_rs_info->koverline[cdm_id] + kp) % frame_parms->ofdm_symbol_size;

            // loop over time resource elements within a group
            for (int lp = 0; lp <= nr_csi_rs_info->lprime; lp++) {
              uint16_t symb = lp + nr_csi_rs_info->loverline[cdm_id];
              uint64_t symbol_offset = symb*frame_parms->ofdm_symbol_size;
              int16_t *tx_csi_rs_signal = (int16_t*)&nr_csi_rs_info->csi_rs_generated_signal[port_tx][symbol_offset+dataF_offset];
              int16_t *rx_csi_rs_signal = (int16_t*)&csi_rs_received_signal[ant_rx][symbol_offset];
              int16_t *csi_rs_ls_estimated_channel = (int16_t*)&nr_csi_rs_info->csi_rs_ls_estimated_channel[ant_rx][port_tx][symbol_offset];

              csi_rs_ls_estimated_channel[k<<1] = (int16_t)(((int32_t)tx_csi_rs_signal[k<<1]*rx_csi_rs_signal[k<<1] + (int32_t)tx_csi_rs_signal[(k<<1)+1]*rx_csi_rs_signal[(k<<1)+1])>>nr_csi_rs_info->csi_rs_generated_signal_bits);
              csi_rs_ls_estimated_channel[(k<<1)+1] = (int16_t)(((int32_t)tx_csi_rs_signal[k<<1]*rx_csi_rs_signal[(k<<1)+1] - (int32_t)tx_csi_rs_signal[(k<<1)+1]*rx_csi_rs_signal[k<<1])>>nr_csi_rs_info->csi_rs_generated_signal_bits);

#ifdef NR_CSIRS_DEBUG
              LOG_I(NR_PHY, "l,k (%2d,%4d) |\tport_tx %d (%4d,%4d)\tant_rx %d (%4d,%4d)\tls (%4d,%4d)\n",
                    symb,
                    k,
                    port_tx+3000,
                    tx_csi_rs_signal[k<<1],
                    tx_csi_rs_signal[(k<<1)+1],
                    ant_rx,
                    rx_csi_rs_signal[k<<1],
                    rx_csi_rs_signal[(k<<1)+1],
                    csi_rs_ls_estimated_channel[k<<1],
                    csi_rs_ls_estimated_channel[(k<<1)+1]);
#endif
            }
          }
        }
      }
    }

    /// Channel interpolation

    for(uint16_t port_tx = 0; port_tx<nr_csi_rs_info->N_ports; port_tx++) {
      memset(csi_rs_estimated_channel_freq[ant_rx][port_tx], 0, frame_parms->ofdm_symbol_size*sizeof(int32_t));
    }

    int16_t ls_estimated[2];

    for (int rb = csirs_config_pdu->start_rb; rb < (csirs_config_pdu->start_rb+csirs_config_pdu->nr_of_rbs); rb++) {

      // for freq density 0.5 checks if even or odd RB
      if(csirs_config_pdu->freq_density <= 1 && csirs_config_pdu->freq_density != (rb % 2)) {
        continue;
      }

      // TODO: Extend this to multiple antenna ports
      int cdm_id = 0;
      int s = 0;
      int kp = 0;
      int lp = 0;

      uint16_t port_tx = s+nr_csi_rs_info->j[cdm_id]*nr_csi_rs_info->CDM_group_size;

      uint16_t k = (frame_parms->first_carrier_offset + (rb*NR_NB_SC_PER_RB)+nr_csi_rs_info->koverline[cdm_id] + kp) % frame_parms->ofdm_symbol_size;
      uint16_t symb = lp + nr_csi_rs_info->loverline[cdm_id];
      uint64_t symbol_offset = symb*frame_parms->ofdm_symbol_size;

      int16_t *csi_rs_ls_estimated_channel = (int16_t*)&nr_csi_rs_info->csi_rs_ls_estimated_channel[ant_rx][port_tx][symbol_offset];
      int16_t *csi_rs_estimated_channel16 = (int16_t *)&csi_rs_estimated_channel_freq[ant_rx][port_tx][k];

      ls_estimated[0] = (int16_t) csi_rs_ls_estimated_channel[k<<1];
      ls_estimated[1] = (int16_t) csi_rs_ls_estimated_channel[(k<<1)+1];

      if( (k == 0) || (k == frame_parms->first_carrier_offset) ) { // Start of OFDM symbol case or first occupied subcarrier case
        multadd_real_vector_complex_scalar(filt24_start, ls_estimated, csi_rs_estimated_channel16, 24);
      } else if( ( (k + NR_NB_SC_PER_RB) >= frame_parms->ofdm_symbol_size) ||
                   (rb == (csirs_config_pdu->start_rb+csirs_config_pdu->nr_of_rbs-1)) ) { // End of OFDM symbol case or Last occupied subcarrier case
        multadd_real_vector_complex_scalar(filt24_end, ls_estimated, csi_rs_estimated_channel16 - 3*sizeof(uint64_t), 24);
      } else { // Middle case
        multadd_real_vector_complex_scalar(filt24_middle, ls_estimated, csi_rs_estimated_channel16 - 3*sizeof(uint64_t), 24);
      }
    }

#ifdef NR_CSIRS_DEBUG

    int cdm_id = 0;
    int s = 0;
    int lp = 0;
    uint16_t port_tx = s+nr_csi_rs_info->j[cdm_id]*nr_csi_rs_info->CDM_group_size;
    uint16_t symb = lp + nr_csi_rs_info->loverline[cdm_id];
    uint64_t symbol_offset = symb*frame_parms->ofdm_symbol_size;
    int16_t *csi_rs_ls_estimated_channel = (int16_t*)&nr_csi_rs_info->csi_rs_ls_estimated_channel[ant_rx][port_tx][symbol_offset];
    int16_t *csi_rs_estimated_channel16 = (int16_t *)&csi_rs_estimated_channel_freq[ant_rx][port_tx][0];

    for(int k = 0; k<frame_parms->ofdm_symbol_size; k++) {
      LOG_I(NR_PHY, "(%4d) |\tls (%4d,%4d)\tint (%4d,%4d)\n",
            k,
            csi_rs_ls_estimated_channel[k<<1],
            csi_rs_ls_estimated_channel[(k<<1)+1],
            csi_rs_estimated_channel16[k<<1],
            csi_rs_estimated_channel16[(k<<1)+1]);
    }

#endif

  }

  return 0;
}

int nr_ue_csi_im_procedures(PHY_VARS_NR_UE *ue, UE_nr_rxtx_proc_t *proc, uint8_t gNB_id) {
  return 0;
}

int nr_ue_csi_rs_procedures(PHY_VARS_NR_UE *ue, UE_nr_rxtx_proc_t *proc, uint8_t gNB_id) {

  if(!ue->csirs_vars[gNB_id]->active) {
    return -1;
  }

  fapi_nr_dl_config_csirs_pdu_rel15_t *csirs_config_pdu = (fapi_nr_dl_config_csirs_pdu_rel15_t*)&ue->csirs_vars[gNB_id]->csirs_config_pdu;

#ifdef NR_CSIRS_DEBUG
  LOG_I(NR_PHY, "csirs_config_pdu->subcarrier_spacing = %i\n", csirs_config_pdu->subcarrier_spacing);
  LOG_I(NR_PHY, "csirs_config_pdu->cyclic_prefix = %i\n", csirs_config_pdu->cyclic_prefix);
  LOG_I(NR_PHY, "csirs_config_pdu->start_rb = %i\n", csirs_config_pdu->start_rb);
  LOG_I(NR_PHY, "csirs_config_pdu->nr_of_rbs = %i\n", csirs_config_pdu->nr_of_rbs);
  LOG_I(NR_PHY, "csirs_config_pdu->csi_type = %i (0:TRS, 1:CSI-RS NZP, 2:CSI-RS ZP)\n", csirs_config_pdu->csi_type);
  LOG_I(NR_PHY, "csirs_config_pdu->row = %i\n", csirs_config_pdu->row);
  LOG_I(NR_PHY, "csirs_config_pdu->freq_domain = %i\n", csirs_config_pdu->freq_domain);
  LOG_I(NR_PHY, "csirs_config_pdu->symb_l0 = %i\n", csirs_config_pdu->symb_l0);
  LOG_I(NR_PHY, "csirs_config_pdu->symb_l1 = %i\n", csirs_config_pdu->symb_l1);
  LOG_I(NR_PHY, "csirs_config_pdu->cdm_type = %i\n", csirs_config_pdu->cdm_type);
  LOG_I(NR_PHY, "csirs_config_pdu->freq_density = %i (0: dot5 (even RB), 1: dot5 (odd RB), 2: one, 3: three)\n", csirs_config_pdu->freq_density);
  LOG_I(NR_PHY, "csirs_config_pdu->scramb_id = %i\n", csirs_config_pdu->scramb_id);
  LOG_I(NR_PHY, "csirs_config_pdu->power_control_offset = %i\n", csirs_config_pdu->power_control_offset);
  LOG_I(NR_PHY, "csirs_config_pdu->power_control_offset_ss = %i\n", csirs_config_pdu->power_control_offset_ss);
#endif

  nr_generate_csi_rs(ue->frame_parms,
                     ue->nr_csi_rs_info->csi_rs_generated_signal,
                     AMP,
                     ue->nr_csi_rs_info,
                     (nfapi_nr_dl_tti_csi_rs_pdu_rel15_t *) csirs_config_pdu,
                     ue->frame_parms.first_carrier_offset,
                     proc->nr_slot_rx);

  nr_get_csi_rs_signal(ue,
                       proc,
                       csirs_config_pdu,
                       ue->nr_csi_rs_info,
                       ue->nr_csi_rs_info->csi_rs_received_signal);

  nr_csi_rs_channel_estimation(ue,
                               proc,
                               csirs_config_pdu,
                               ue->nr_csi_rs_info,
                               ue->nr_csi_rs_info->csi_rs_generated_signal,
                               ue->nr_csi_rs_info->csi_rs_received_signal,
                               ue->nr_csi_rs_info->csi_rs_estimated_channel_freq,
                               ue->nr_csi_rs_info->noise_power);
  return 0;
}
