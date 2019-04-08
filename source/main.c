#include <3ds.h>
#include <stdio.h>
#include <ecl/ecl.h>

int main(int argc, char** argv)
{
  // Initializations
  cl_boot(argc, argv); // Boot Embeddable Common Lisp 
  srvInit();        // services
  aptInit();        // applets
  hidInit();        // input
  gfxInitDefault(); // graphics
  gfxSet3D(false);  // stereoscopy (true: enabled / false: disabled)
  consoleInit(GFX_TOP, NULL);
  u32 kDown;        // keys down
  u32 kHeld;        // keys pressed
  u32 kUp;          // keys up

  printf("3DS Lisp REPL -- version 0.0.1\n");
  printf("Press START to exit\n");
  printf("Press A to bring up the keyboard\n\n");
  printf("* ");

  // Main loop
  while (aptMainLoop())
  {
    // Wait for next frame
    gspWaitForVBlank();

    // Read which buttons are currently pressed or not
    hidScanInput();
    kDown = hidKeysDown();
    kHeld = hidKeysHeld();
    kUp = hidKeysUp();

    // If START button is pressed, break loop and quit
    if (kDown & KEY_START) {
      break;
    }

    static SwkbdState swkbd; // init keyboard
    static char input[60];   // set max input buffer
    cl_object result;          // output for evaluated code
    static SwkbdStatusData swkbdStatus;
    static SwkbdLearningData swkbdLearning;
    SwkbdButton button = SWKBD_BUTTON_NONE;

    if (kDown & KEY_A) {
        swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 3, -1);
        swkbdSetInitialText(&swkbd, input);
        swkbdSetHintText(&swkbd, "3DScheme REPL");

        swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Exit", false);
        swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Execute", true);

        swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT); // autofill input

        swkbdSetStatusData(&swkbd, &swkbdStatus, false, true);
        swkbdSetLearningData(&swkbd, &swkbdLearning, false, true);

        button = swkbdInputText(&swkbd, input, sizeof(input));

        printf("%s\n", input); // print input
        result = cl_eval(c_string_to_object(input));
        ecl_print(result, ECL_T); // print input and start a new line

        // reset input
        for (int i = 0; i < 60; i++) {
            input[i] = ' ';
        }

        result = 0;
    } 

    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxSwapBuffers();
  }

  // Exit
  gfxExit();
  hidExit();
  aptExit();
  srvExit();
  cl_shutdown();

  // Return to hbmenu
  return 0;
}