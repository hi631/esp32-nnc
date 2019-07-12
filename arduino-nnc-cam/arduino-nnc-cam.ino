
extern void cam_setup();
extern void cam_loop();
extern void nnc_loop();

void setup() {
  cam_setup();
}


void loop() {
  cam_loop();
  //nnc_loop();
}
