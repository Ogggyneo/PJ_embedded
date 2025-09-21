#include "dht_anomaly_model.h"
#include <DHT20.h>
#include <TensorFlowLite_ESP32.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"


DHT20 dht20;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 2 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

const tflite::Model* model = tflite::GetModel(dht_anomaly_model_tflite);
tflite::MicroInterpreter* interpreter = nullptr;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  dht20.begin();

  static tflite::MicroMutableOpResolver<5> resolver;
  resolver.AddFullyConnected();
  resolver.AddReshape();
  resolver.AddLogistic();
  resolver.AddSoftmax();
  resolver.AddQuantize();

  interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena, kTensorArenaSize, nullptr);
  interpreter->AllocateTensors();

  input = interpreter->input(0);
  output = interpreter->output(0);
}

void loop() {
  float temp = 20;
  float hum = 50;

  input->data.f[0] = temp;
  input->data.f[1] = hum;

  interpreter->Invoke();

  float anomaly_prob = output->data.f[0];
  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(", Hum: "); Serial.print(hum);
  Serial.print(", Anomaly probability: "); Serial.println(anomaly_prob);

  if (anomaly_prob > 0.5) {
    Serial.println("Abnormality detected!");
    // Add alarms, LED, etc.
  }
  delay(2000);
}