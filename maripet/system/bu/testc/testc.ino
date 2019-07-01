#include <nRF5x_BLE_API.h>
BLE           ble;

//ServerのUUID
static uint8_t service1_uuid[]    = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
UUID service_uuid(service1_uuid);

static uint8_t device_is_simple_peripheral = 0;

// When found the match characteristic, set 1.
static uint8_t characteristic_is_fond = 0;
// When found the match descriptor, set 1.
static uint8_t descriptor_is_found = 0;
// To save the hrm characteristic and descriptor
static DiscoveredCharacteristic            Characteristic_values;
static DiscoveredCharacteristicDescriptor  desc_of_Characteristic_values(NULL,GattAttribute::INVALID_HANDLE,GattAttribute::INVALID_HANDLE,UUID::ShortUUIDBytes_t(0));

//関数
static void scanCallBack(const Gap::AdvertisementCallbackParams_t *params);
static void discoveredCharacteristicCallBack(const DiscoveredCharacteristic *chars);
static void discoveryTerminationCallBack(Gap::Handle_t connectionHandle);
static void discoveredCharsDescriptorCallBack(const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t *params);
static void discoveredDescTerminationCallBack(const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t *params) ;

static void onDataWriteCallBack(const GattWriteCallbackParams *params); //送信のときに呼ぶ
static void onDataReadCallBack(const GattReadCallbackParams *params); //静的なデータを取得するときに呼ぶ
void hvxCallBack(const GattHVXCallbackParams *params); //動的なデータを取得するときに呼ぶ


/**
 * 接続判定をする関数
 */
uint32_t ble_advdata_parser(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data) {
  uint8_t index=0;
  uint8_t field_length, field_type;

  while(index<advdata_len) {
    field_length = p_advdata[index];
    field_type   = p_advdata[index+1];
    if(field_type == type) {
      memcpy(p_field_data, &p_advdata[index+2], (field_length-1));
      *len = field_length - 1;
      return NRF_SUCCESS;
    }
    index += field_length + 1;
  }
  return NRF_ERROR_NOT_FOUND;
}


/**
 * スキャンの後、ショートネームが一致したら接続
 */
static void scanCallBack(const Gap::AdvertisementCallbackParams_t *params) {
  uint8_t len;
  uint8_t adv_name[31];
//  if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) ) {
//    Serial.print("Short name len : ");
//    Serial.println(len, DEC);
//    Serial.print("Short name is : ");
//    Serial.println((const char*)adv_name);
//
//    if( memcmp("TXRX", adv_name, 4) == 0x00 ) {//取得したいショートネームを書く
//      Serial.println("Got device, stop scan ");
//      ble.stopScan();
//      device_is_simple_peripheral = 1;
//      ble.connect(params->peerAddr, BLEProtocol::AddressType::RANDOM_STATIC, NULL, NULL);
//    }
//  }

  if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) ) {
    

    if(memcmp("OUTER", adv_name, len) == 0x00) {
      Serial.print("Complete name is : ");
      Serial.println((const char*)adv_name);
      ble.stopScan();
      device_is_simple_peripheral = 1;
      ble.connect(params->peerAddr, BLEProtocol::AddressType::RANDOM_STATIC, NULL, NULL);
      Serial.println("Got device, stop scan Mobile");

    }
  }
  Serial.println(" ");
}

/** 
 * 接続した後、振る舞い(write, read etc..)の情報を取得
 * discoveredCharacteristicCallBack関数で表示をする
 */
void connectionCallBack( const Gap::ConnectionCallbackParams_t *params ) {
  Serial.println("\r\n----startDiscovery");
  //オプションつけると自作したサービスのみが呼ばれるようになる
  //ble.gattClient().launchServiceDiscovery(params->handle, NULL, discoveredCharacteristicCallBack);
  ble.gattClient().launchServiceDiscovery(params->handle, NULL, discoveredCharacteristicCallBack, service_uuid);
}


/*
 * 切断のときに呼ばれる
 */
void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  Serial.println("Disconnected, start to scanning");
  device_is_simple_peripheral = 0;
  characteristic_is_fond = 0;
  descriptor_is_found = 0;
  ble.startScan(scanCallBack);
}

/**
 * 接続されたBLEのRSSIやUUIDを表示する
 */
static void discoveredCharacteristicCallBack(const DiscoveredCharacteristic *chars) {
  //Info
  Serial.print("Chars UUID type:");
  Serial.println(chars->getUUID().shortOrLong(), HEX);// 0 16bit_uuid, 1 128bit_uuid
  Serial.print("Chars UUID: ");
  if(chars->getUUID().shortOrLong() == UUID::UUID_TYPE_SHORT) {//すでに決まっているUUIDを列挙
    Serial.println(chars->getUUID().getShortUUID(), HEX);
  }
  else {//作ったUUIDを列挙
    uint8_t index;
    const uint8_t *uuid = chars->getUUID().getBaseUUID();
    for(index=0; index<16; index++) {
      Serial.print(uuid[index], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.print("properties_read: ");
  Serial.println(chars->getProperties().read(), DEC);
  Serial.print("properties_writeWoResp : ");
  Serial.println(chars->getProperties().writeWoResp(), DEC);
  Serial.print("properties_write: ");
  Serial.println(chars->getProperties().write(), DEC);
  Serial.print("properties_notify: ");
  Serial.println(chars->getProperties().notify(), DEC);
  Serial.print("declHandle: ");
  Serial.println(chars->getDeclHandle(), HEX);
  Serial.print("valueHandle: ");
  Serial.println(chars->getValueHandle(), HEX);
  Serial.print("lastHandle: ");
  Serial.println(chars->getLastHandle(), HEX);
  Serial.print("getConnectionHandle: ");
  Serial.println(chars->getConnectionHandle(), HEX);
  Serial.println("");

  //notifyが許可されているか
  if(chars->getProperties().notify() == 0x01){
    Serial.println("found notify");
    characteristic_is_fond = 1;
    Characteristic_values = *chars;
  }
}

/**
 * 通知の準備
 */
static void discoveryTerminationCallBack(Gap::Handle_t connectionHandle) {
  Serial.println("\r\n----discoveryTermination");
  Serial.print("connectionHandle       : ");
  Serial.println(connectionHandle, HEX);
  if(characteristic_is_fond == 1) {
    ble.gattClient().discoverCharacteristicDescriptors(Characteristic_values, discoveredCharsDescriptorCallBack, discoveredDescTerminationCallBack);
  }
}

/** 
 *  0x2902(通知のUUID)があるか
 */
static void discoveredCharsDescriptorCallBack(const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t *params) {
  Serial.println("\r\n----discovered descriptor");
  Serial.print("Desriptor UUID : ");
  Serial.println(params->descriptor.getUUID().getShortUUID(), HEX);
  if(params->descriptor.getUUID().getShortUUID() == 0x2902) {
    // Save characteristic info
    descriptor_is_found = 1;
    desc_of_Characteristic_values = params->descriptor;
  }
}

/** 
 * クライアントに送信して、notifyの準備をしてもらう
 */
static void discoveredDescTerminationCallBack(const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t *params) {
  Serial.println("\r\n----discovery descriptor Termination");
  if(descriptor_is_found) {
    uint16_t value = 0x0001;
    ble.gattClient().write(GattClient::GATT_OP_WRITE_REQ, Characteristic_values.getConnectionHandle(), desc_of_Characteristic_values.getAttributeHandle(), 2, (uint8_t *)&value);
  }
}

/**
 * データをPeripheralへ送信
 * Peripheralに何かしてほしいときはこの関数をコールバックする。
 */
void onDataWriteCallBack(const GattWriteCallbackParams *params) {
  Serial.println("GattClient write call back ");
}

/** @brief  read callback handle
 *データの読み込み
 *Peripheralからのデータを読み込み
 *notifyと比較して、静的なデータのみを読み込む
 */
void onDataReadCallBack(const GattReadCallbackParams *params) {
  Serial.println("GattClient read call back ");
  Serial.print("The handle : ");
  Serial.println(params->handle, HEX);
  Serial.print("The offset : ");
  Serial.println(params->offset, DEC);
  Serial.print("The len : ");
  Serial.println(params->len, DEC);
  Serial.print("The data : ");
  for(uint8_t index=0; index<params->len; index++) {
    Serial.print( params->data[index]); //これでデータを取得する
    Serial.print(" ");
    }
  Serial.println("");
}

/**
 * notifyの読み込み
 * Peripheralから動的なデータを取得するときに使う
 */
void hvxCallBack(const GattHVXCallbackParams *params) {
  Serial.println("GattClient notify call back ");
  Serial.print("The len : ");
  Serial.println(params->len, DEC);
  for(unsigned char index=0; index<params->len; index++) {
    //Serial.print(params->data[index], HEX);
    Serial.print(params->data[index]);
    Serial.print(" ");
  }
  Serial.println("");
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("BLE Central Demo ");

  ble.init();
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.onConnection(connectionCallBack);
  ble.onDisconnection(disconnectionCallBack); //切断されたときの処理
  ble.gattClient().onServiceDiscoveryTermination(discoveryTerminationCallBack); //通知
  ble.gattClient().onHVX(hvxCallBack);
  ble.gattClient().onDataWrite(onDataWriteCallBack); //書き込み send
  ble.gattClient().onDataRead(onDataReadCallBack); //読み込み read
  // scan interval : in milliseconds, valid values lie between 2.5ms and 10.24s
  // scan window :in milliseconds, valid values lie between 2.5ms and 10.24s
  // timeout : in seconds, between 0x0001 and 0xFFFF, 0x0000 disables timeout
  // activeScanning : true or false
  ble.setScanParams(1000, 200, 0, true); //スキャニング時間 ウィンドウ時間? タイムアウト アクティブにするか
  // start scanning
  ble.startScan(scanCallBack); //スキャニングのスタート

  pinMode(13,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  ble.waitForEvent();
}
