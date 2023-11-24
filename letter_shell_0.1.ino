#include "shell.h"
Shell shell;
void setup() {
  // put your setup code here, to run once:
  shell.init(115200);
}

void loop() {
  shell.loop();
}