/*
 * gatts_handler.h
 * Handler for GATT-Server related operations
 */

#pragma once
// Block of libraries for ESP functionality
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#include "gatt_def.h"

// Core Interface
#include "core_int.h"


/*
 * gatts_profile_inst represent an Application Profile object
 * used to establish a GATT connection with one client application
 * (In our case, the bemesh network).
 * The structure is taken from the GATTS example provided by ESP-IDF.
 * The structure contains a single service with a single characteristic,
 * which is exactly the kind of structure we're looking for.
 * PLEASE NOTICE:
 * The structure is slightly different from the example's one, since 
 * BEMESH protocol wont rely on multiple profiles. A single GATTS callback
 * is used, therefore the gatts_cb element was removed.
 */
typedef struct {
  // Unused since we're using a single profile
  // esp_gatts_cb_t gatts_cb;
  // gatts interface (provided by the bt_driver)
  uint16_t gatts_if;
  // application id
  uint16_t app_id;
  // connection id
  uint16_t conn_id;
  uint16_t service_handle;
  esp_gatt_srvc_id_t service_id;
  uint16_t char_handle;
  esp_bt_uuid_t char_uuid;
  esp_gatt_perm_t perm;
  esp_gatt_char_prop_t property;
  uint16_t descr_handle;
  esp_bt_uuid_t descr_uuid;
} gatts_profile_inst;

/*
 * In order to communicate with higher levels, the gatts_handler requires a callback handler
 */

typedef enum {
  O_IGNCONN = 1<<0,  // Ignore connection event (used during gattc_open actions)
} BemeshGattsFlags;

struct bemesh_gatts_handler;

typedef struct bemesh_gatts_handler {
  gatts_profile_inst profile_inst;
  esp_attr_value_t char1_val;
  uint8_t char_buffer[GATT_CHAR_BUF_SIZE];
  // Response structure, used during write events
  esp_gatt_rsp_t gatt_rsp;
  // exec_write_buffer
  uint8_t exec_write_buffer[EXEC_WRITE_BUF_LEN];
  uint8_t exec_write_len;

  uint8_t flags;

  // Callback to pass events to the core lib.
  kernel_cb core_cb;
  // It will rely on params struct built inside the core.
  bemesh_evt_params_t *core_cb_args;
} bemesh_gatts_handler;

bemesh_gatts_handler* bemesh_gatts_handler_init(void);

void bemesh_gatts_handler_install_cb(bemesh_gatts_handler *h,
                                     kernel_cb cb, bemesh_evt_params_t *params);

void bemesh_gatts_handler_uninstall_cb(bemesh_gatts_handler *h);

void bemesh_gatts_handler_send_notify(bemesh_gatts_handler *h,
                                      uint16_t conn_id,
                                      uint8_t *data,
                                      uint16_t data_len);
