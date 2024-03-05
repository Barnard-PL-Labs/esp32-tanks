// to try: 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 23, 24, 25, 26, 27, 38, 39, 40, 41, 42, 43, 51, 52
// CM pins on case_code > camera_init

int GPIO_PINS[] = {7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 23, 24, 25, 26, 27, 38, 39, 40, 41, 42, 43, 51, 52};
size_t num_pins = sizeof(GPIO_PINS) / sizeof(GPIO_PINS[0]);

void setup() {
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < num_pins; i++) {
    Serial.print("Pin_");
    Serial.print(GPIO_PINS[i]);
    Serial.print(":");
    Serial.print(analogRead(GPIO_PINS[i]));
    Serial.print(", ");
  }
  Serial.println("");
}
