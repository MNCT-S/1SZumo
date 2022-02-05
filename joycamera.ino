/*********
  Camera + JoyStick
  https://github.com/bobboteck/JoyStick
  https://gist.github.com/me-no-dev/d34fba51a8f059ac559bf62002e61aa3
*********/

#include <WiFi.h>
#include "esp_camera.h"
#include "ESPAsyncWebServer.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include "SPIFFS.h"

const char* ssid     = "ESP32-Access-Point";
const char* password = "mncts-12345";
const IPAddress ip(192, 168, 10, 11);
const IPAddress subnet(255, 255, 255, 0);

typedef struct {
  camera_fb_t * fb;
  size_t index;
} camera_frame_t;

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* STREAM_PART = "Content-Type: %s\r\nContent-Length: %u\r\n\r\n";

static const char * JPG_CONTENT_TYPE = "image/jpeg";

class AsyncBufferResponse: public AsyncAbstractResponse {
  private:
    uint8_t * _buf;
    size_t _len;
    size_t _index;
  public:
    AsyncBufferResponse(uint8_t * buf, size_t len, const char * contentType) {
      _buf = buf;
      _len = len;
      _callback = nullptr;
      _code = 200;
      _contentLength = _len;
      _contentType = contentType;
      _index = 0;
    }
    ~AsyncBufferResponse() {
      if (_buf != nullptr) {
        free(_buf);
      }
    }
    bool _sourceValid() const {
      return _buf != nullptr;
    }
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override {
      size_t ret = _content(buf, maxLen, _index);
      if (ret != RESPONSE_TRY_AGAIN) {
        _index += ret;
      }
      return ret;
    }
    size_t _content(uint8_t *buffer, size_t maxLen, size_t index) {
      memcpy(buffer, _buf + index, maxLen);
      if ((index + maxLen) == _len) {
        free(_buf);
        _buf = nullptr;
      }
      return maxLen;
    }
};

class AsyncFrameResponse: public AsyncAbstractResponse {
  private:
    camera_fb_t * fb;
    size_t _index;
  public:
    AsyncFrameResponse(camera_fb_t * frame, const char * contentType) {
      _callback = nullptr;
      _code = 200;
      _contentLength = frame->len;
      _contentType = contentType;
      _index = 0;
      fb = frame;
    }
    ~AsyncFrameResponse() {
      if (fb != nullptr) {
        esp_camera_fb_return(fb);
      }
    }
    bool _sourceValid() const {
      return fb != nullptr;
    }
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override {
      size_t ret = _content(buf, maxLen, _index);
      if (ret != RESPONSE_TRY_AGAIN) {
        _index += ret;
      }
      return ret;
    }
    size_t _content(uint8_t *buffer, size_t maxLen, size_t index) {
      memcpy(buffer, fb->buf + index, maxLen);
      if ((index + maxLen) == fb->len) {
        esp_camera_fb_return(fb);
        fb = nullptr;
      }
      return maxLen;
    }
};

class AsyncJpegStreamResponse: public AsyncAbstractResponse {
  private:
    camera_frame_t _frame;
    size_t _index;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    uint64_t lastAsyncRequest;
  public:
    AsyncJpegStreamResponse() {
      _callback = nullptr;
      _code = 200;
      _contentLength = 0;
      _contentType = STREAM_CONTENT_TYPE;
      _sendContentLength = false;
      _chunked = true;
      _index = 0;
      _jpg_buf_len = 0;
      _jpg_buf = NULL;
      lastAsyncRequest = 0;
      memset(&_frame, 0, sizeof(camera_frame_t));
    }
    ~AsyncJpegStreamResponse() {
      if (_frame.fb) {
        if (_frame.fb->format != PIXFORMAT_JPEG) {
          free(_jpg_buf);
        }
        esp_camera_fb_return(_frame.fb);
      }
    }
    bool _sourceValid() const {
      return true;
    }
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override {
      size_t ret = _content(buf, maxLen, _index);
      if (ret != RESPONSE_TRY_AGAIN) {
        _index += ret;
      }
      return ret;
    }
    size_t _content(uint8_t *buffer, size_t maxLen, size_t index) {
      if (!_frame.fb || _frame.index == _jpg_buf_len) {
        if (index && _frame.fb) {
          uint64_t end = (uint64_t)micros();
          int fp = (end - lastAsyncRequest) / 1000;
          log_printf("Size: %uKB, Time: %ums (%.1ffps)\n", _jpg_buf_len / 1024, fp);
          lastAsyncRequest = end;
          if (_frame.fb->format != PIXFORMAT_JPEG) {
            free(_jpg_buf);
          }
          esp_camera_fb_return(_frame.fb);
          _frame.fb = NULL;
          _jpg_buf_len = 0;
          _jpg_buf = NULL;
        }
        if (maxLen < (strlen(STREAM_BOUNDARY) + strlen(STREAM_PART) + strlen(JPG_CONTENT_TYPE) + 8)) {
          //log_w("Not enough space for headers");
          return RESPONSE_TRY_AGAIN;
        }
        //get frame
        _frame.index = 0;

        _frame.fb = esp_camera_fb_get();
        if (_frame.fb == NULL) {
          log_e("Camera frame failed");
          return 0;
        }

        if (_frame.fb->format != PIXFORMAT_JPEG) {
          unsigned long st = millis();
          bool jpeg_converted = frame2jpg(_frame.fb, 80, &_jpg_buf, &_jpg_buf_len);
          if (!jpeg_converted) {
            log_e("JPEG compression failed");
            esp_camera_fb_return(_frame.fb);
            _frame.fb = NULL;
            _jpg_buf_len = 0;
            _jpg_buf = NULL;
            return 0;
          }
          log_i("JPEG: %lums, %uB", millis() - st, _jpg_buf_len);
        } else {
          _jpg_buf_len = _frame.fb->len;
          _jpg_buf = _frame.fb->buf;
        }

        //send boundary
        size_t blen = 0;
        if (index) {
          blen = strlen(STREAM_BOUNDARY);
          memcpy(buffer, STREAM_BOUNDARY, blen);
          buffer += blen;
        }
        //send header
        size_t hlen = sprintf((char *)buffer, STREAM_PART, JPG_CONTENT_TYPE, _jpg_buf_len);
        buffer += hlen;
        //send frame
        hlen = maxLen - hlen - blen;
        if (hlen > _jpg_buf_len) {
          maxLen -= hlen - _jpg_buf_len;
          hlen = _jpg_buf_len;
        }
        memcpy(buffer, _jpg_buf, hlen);
        _frame.index += hlen;
        return maxLen;
      }

      size_t available = _jpg_buf_len - _frame.index;
      if (maxLen > available) {
        maxLen = available;
      }
      memcpy(buffer, _jpg_buf + _frame.index, maxLen);
      _frame.index += maxLen;

      return maxLen;
    }
};

void streamJpg(AsyncWebServerRequest *request) {
  AsyncJpegStreamResponse *response = new AsyncJpegStreamResponse();
  if (!response) {
    request->send(501);
    return;
  }
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}

void softap_connect(const char *ssid, const char *password, IPAddress ip, IPAddress subnet) {
  Serial.println("Setting AP (Access Point)…");
  WiFi.softAPConfig(ip, ip, subnet);
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  Serial.print("SSID:");
  Serial.println(ssid);
  Serial.print("Password:");
  Serial.println(password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Camera Stream Ready! Connect to the ESP32 AP and go to: http://");
  Serial.println(myIP);
}

void initcamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //Setup size of image
  if (psramFound()) {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  String msg = "";
  AwsFrameInfo * info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    for (size_t i = 0; i < info->len; i++) {
      msg += (char) data[i];
    }
  }
  Serial.println(msg); // GamePad の座標をシリアルに
  int xyIndex = msg.indexOf(',');
  int x = msg.substring(0, xyIndex);
  int y = msg.substring(xyIndex + 1);
  Serial.println("X:" + x);
  Serial.println("Y:" + y);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    //Serial.println("Websocket client connection received");
    Serial.printf("ws(url=%s,id=%u) connect\n", server->url(), client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
  } else if (type == WS_EVT_DATA) {
    handleWebSocketMessage(arg, data, len);
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
   
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  initcamera();
  softap_connect(ssid, password, ip, subnet);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  // Route to joy.min.js file
  server.on("/joy.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/joy.min.js", "application/javascript");
  });
  server.on("/stream", HTTP_GET, streamJpg);
  //server.serveStatic("/", SPIFFS, "/");

  // WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/stream", HTTP_GET, streamJpg);
  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
}
