/*
 * core_int.c
 */

#include "core_int.h"
#include "core.h"
#include "esp_log.h"

static bemesh_core_t *core;
static const char *TAG="core_interface";

/*
 * Install the cb callback for the Event event.
 * Parameters will be updated before launching the cb
 * Refer to bemesh_kernel_evt_t enum to check how events are defined.
 * Refer to bemesh_evt_params_t union to check how params are passed.
 */
void kernel_install_cb(bemesh_kernel_evt_t event, kernel_int_cb cb) {
  bemesh_core_install_callback(core, event, cb);
  ESP_LOGI(TAG, "installing callback for event %d.", event);
}

/**
 * Uninstall the callback associated with event, if its present.
 *
 * @param event
 */
void kernel_uninstall_cb(bemesh_kernel_evt_t event) {
  bemesh_core_uninstall_callback(core, event);
  ESP_LOGI(TAG, "uninstalling callback for event %d.", event);
}

/*
 * Initializes the underlying kernel.
 * This function has to be called in the startup of the system.
 */
int kernel_init(void) {
  // SET LOGGING LEVEL TO WARNING
  //esp_log_level_set(TAG, ESP_LOG_WARN);
  ESP_LOGI(TAG, "initializing kernel.");
  // initializes the core structure.
  core = bemesh_core_init();
  if (!core) {
    // If something goes wrong, return an error
    return 1;
  }
  return 0;
}


static bda_id_tuple *find_tuple(bda_id_tuple* vect,
                                uint8_t len, esp_bd_addr_t bda,
                                uint8_t *found_flag) {
  for (int i = 0; i < len; ++i) {
    bda_id_tuple *entry = &vect[i];
    if (bda_equals(bda, entry->bda)) {
      *found_flag = true;
      return entry;
    }
  }
  *found_flag = false;
  return NULL;
}

/**
 *  Transfer the src buffer of len bytes to another device with bda address.
 *
 * @param bda address of the device that has to receive the payload
 * @param src array that contains the payload written in byte
 * @param len length of the payload
 */
void send_payload(esp_bd_addr_t bda, uint8_t *src, uint16_t len) {
  ESP_LOGI(TAG, "send_payload called: bda:");
  ESP_LOG_BUFFER_HEX(TAG, (uint8_t *)bda, ESP_BD_ADDR_LEN);
  // search for the given bda in the core entries.
  uint8_t notify_flag = false;
  uint16_t conn_id = CORE_UNUSED_CONN_ID;
  for (int i = 0; i < GATTC_MAX_CONNECTIONS; ++i) {
    bda_id_tuple *entry=&core->outgoing_conn[i];
    if (bda_equals(bda, entry->bda)) {
      conn_id = entry->conn_id;
      break;
    }    
  }
  if(conn_id == CORE_UNUSED_CONN_ID) {
    for(int i = 0; i < GATTS_MAX_CONNECTIONS; ++i) {
      bda_id_tuple *entry=&core->incoming_conn[i];
      if(bda_equals(bda, entry->bda)) {
	notify_flag = true;
	conn_id = entry->conn_id;
	break;
      } 
    }
  }
  if(conn_id != CORE_UNUSED_CONN_ID) {
    ESP_LOGI(TAG, "sending write: notify:%d, len:%d, conn_id:%d",
	     notify_flag, len, conn_id);
    bemesh_core_write(core, conn_id, src, len, notify_flag);
  } else {
    ESP_LOGE(TAG, "Error: could not execute send_payload: No remote dev with bda found.");
  }
  return;
}


/**
 * Tries to connect to a specific device
 *
 * @param bda address
 * @return 0 if no error occurred
 */
uint8_t connect_to(esp_bd_addr_t bda) {
  ESP_LOGI(TAG, "connect_to called.");
  return bemesh_core_connect(core, bda)==ESP_GATT_OK;
}

/**
 * Scan the environment
 *
 * @param timeout length of the scan in seconds
 */
void scan_environment(uint8_t timeout) {
  ESP_LOGI(TAG, "scan_environment called.");
  bemesh_core_start_scanning(core, timeout);
  return;
}

/**
 * Stop the scanning procedure.
 * After the timeout has expired, the scan mode will
 * stop autonomously.
 */
void stop_scan(void) {
  ESP_LOGI(TAG, "stop_scan called.");
  bemesh_core_stop_scanning(core);
  return;
}

/**
 * Returns the state of the GAP module regarding scan.
 * @return 1 if the kernel is in scan mode, 0 otherwise.
 */
uint8_t is_scanning(void) {
  ESP_LOGI(TAG, "is_scanning called.");
  return bemesh_core_is_scanning(core);
}

/**
 * Start the advertising procedure.
 * Advertising and scanning should not be executed
 * concurrently. Check if scanning is in act before
 * starting the advertisment procedure.
 */
void start_advertising(void) {
  ESP_LOGI(TAG, "start_advertising called.");
  bemesh_core_start_advertising(core);
  return;
}

/**
 * Stop the advertising procedure.
 */
void stop_advertising(void) {
  ESP_LOGI(TAG, "stop_advertising called.");
  bemesh_core_stop_advertising(core);
  return;
}

/**
 * Returns the state of the GAP module regarding advertisement.
 * @return 1 if the kernel is in advertisement mode, 0 otherwise.
 */
uint8_t is_advertising(void) {
  ESP_LOGI(TAG, "is_advertising called.");
  return bemesh_core_is_advertising(core);
}

/**
 * Get the device bda. It can be passed to bemesh::to_dev_addr() in order to get the correct address.
 *
 * @return the device bda in bytes
 */
uint8_t *get_own_bda(void) {
  ESP_LOGI(TAG, "get_own_bda called.");
  return bemesh_core_get_bda(core);
}

/**
 * Returns the current number of handled
 * outgoing connections.
 */
uint8_t get_num_out_conn(void) {
  ESP_LOGI(TAG, "get_num_out_conn called.");
  return core->outgoing_conn_len;
}

/**
 * Returns the current number of handled
 * incoming connections.
 */
uint8_t get_num_inc_conn(void) {
  ESP_LOGI(TAG, "get_num_inc_conn called.");
  return core->incoming_conn_len;
}
