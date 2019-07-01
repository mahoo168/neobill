
#include <nRF5x_BLE_API.h>
#define TXRX_BUF_LEN 20
#define DEVICE_NAME       "BLE_Peripheral"

BLE ble;

//Timer
Ticker ticker1s;

//データの格納する配列
static uint8_t tx_buf[TXRX_BUF_LEN];

// The Nordic UART Service
static const uint8_t service1_uuid[]        = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_chars1_uuid[] = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_chars2_uuid[] = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_chars3_uuid[] = {0x71, 0x3D, 0, 4, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[]   = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};
 
uint8_t chars1_value[TXRX_BUF_LEN] = {0,};
uint8_t chars2_value[TXRX_BUF_LEN] = {0,};
uint8_t chars3_value[TXRX_BUF_LEN] = {0x01, 0x02, 0x03};

GattCharacteristic characteristic1(service1_chars1_uuid, chars1_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic characteristic2(service1_chars2_uuid, chars2_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
//通知
GattCharacteristic characteristic3(service1_chars3_uuid, chars3_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic *uartChars[] = {&characteristic1, &characteristic2, &characteristic3};
GattService uartService(service1_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));

//接続がされたときの処理
void connectionCallBack( const Gap::ConnectionCallbackParams_t *params ) {
  Serial.println("Conect");
   if(params->role == Gap::PERIPHERAL) {
    Serial.println("Peripheral ");
  }
}

//切断
void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  Serial.print("Disconnected hande : ");
  Serial.println(params->handle);
  Serial.print("Disconnected reason : ");
  Serial.println(params->reason);
  Serial.println("Restart advertising ");
  ble.startAdvertising();
}

//データの読み込み
void writtenHandle(const GattWriteCallbackParams *Handler) {
  uint8_t buf[TXRX_BUF_LEN];
  uint16_t bytesRead, index;

  if (Handler->handle == characteristic1.getValueAttribute().getHandle()) {
    ble.readCharacteristicValue(characteristic1.getValueAttribute().getHandle(), buf, &bytesRead);
    for(byte index=0; index<bytesRead; index++) {
      Serial.write(buf[index]);
    }
  }
}

void task_handle(void) {
  tx_buf[0] += 2;
  tx_buf[1] += 2;
  tx_buf[2] += 2;

  //BLEデータ送信
  Serial.print(tx_buf[0]); Serial.print("\t");
  Serial.print(tx_buf[1]); Serial.print("\t");
  Serial.println(tx_buf[2]);

  //静的なデータを送信
  //ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), tx_buf, 3);
  
  //動的なデータを送信
  ble.updateCharacteristicValue(characteristic3.getValueAttribute().getHandle(), tx_buf, 3);
}


void setup() {
  Serial.begin(9600);

   // タイマー
  ticker1s.attach(task_handle, 5);

  //BLE設定
  ble.init();
  ble.onConnection(connectionCallBack);
  ble.onDisconnection(disconnectionCallBack);
  ble.onDataWritten(writtenHandle);
      
  // setup adv_data and srp_data
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,(const uint8_t *)"TXRX", sizeof("TXRX") - 1);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,(const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid_rev));
  ble.accumulateScanResponse(GapAdvertisingData::COMPLETE_LOCAL_NAME,(const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);

  // set adv_type
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);    
  // add service
  ble.addService(uartService);
  // set device name
  ble.setDeviceName((const uint8_t *)DEVICE_NAME);
  // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
  ble.setTxPower(4);
  // set adv_interval, 100ms in multiples of 0.625ms.
  ble.setAdvertisingInterval(160);
  // set adv_timeout, in seconds
  ble.setAdvertisingTimeout(0);
  // start advertising
  ble.startAdvertising();

  Serial.println("Advertising Start!"); 

  //initiaze scope
  tx_buf[0] = 50;
  tx_buf[1] = 10;
  tx_buf[2] = 20;
  Serial.print(tx_buf[0]); Serial.print("\t");
  Serial.print(tx_buf[1]); Serial.print("\t");
  Serial.println(tx_buf[2]);
}

void loop() {   
  ble.waitForEvent();
}
