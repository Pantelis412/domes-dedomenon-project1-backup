#include <stdio.h>
#include "include/raylib.h"
#include "include/interface.h"
#include "include/state.h"

State state;

void update_and_draw() {
	state_update(state);
	interface_draw_frame(state);
}