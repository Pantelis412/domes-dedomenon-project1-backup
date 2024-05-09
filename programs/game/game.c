#include <stdio.h>
#include "raylib.h"
#include "interface.h"
#include "state.h"
#include <stdlib.h>

State state;

KeyState keys_init(){
	KeyState keys=malloc(sizeof(*keys));
	keys->up= IsKeyPressed(KEY_UP);
	keys->left= IsKeyPressed(KEY_LEFT);
	keys->right= IsKeyPressed(KEY_RIGHT);
	keys->p= IsKeyPressed(KEY_P);
	keys->n= IsKeyPressed(KEY_N);
	keys->enter= IsKeyPressed(KEY_ENTER);
	keys->space= IsKeyPressed(KEY_SPACE);
	return keys;
}

void update_and_draw() {
	KeyState keys=keys_init();
	state_update(state,keys);
	interface_draw_frame(state);
	free(keys);
}

int main(){
	state =state_create();
	interface_init();

	start_main_loop(update_and_draw);
	interface_close();
	state_destroy(state);
	return 0;
}